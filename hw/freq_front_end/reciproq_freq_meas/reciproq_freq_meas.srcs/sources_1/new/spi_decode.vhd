library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity spi_decode is
  generic (
    FREF_HZ     : natural := 12_000_000;  -- documentation only (used to compute timeout)
    REF_CNT_W   : natural := 64;          -- width of reference tick counter
    MAX_N       : natural := 200_000;     -- maximum allowed N
    TIMEOUT_MS  : natural := 2000         -- timeout in ms (safety)
  );
    port (
        clk     : in  std_logic;
        reset_n : in  std_logic;

        -- From SPI PHY
        rx_word  : in  std_logic_vector(31 downto 0);
        rx_valid : in  std_logic;

        -- To SPI PHY
        tx_word  : out std_logic_vector(31 downto 0);
        tx_load  : out std_logic;
        tx_busy  : in  std_logic;
        
        -- LED
        led_on     : out std_logic;
        led_toggle : out std_logic;
        led_off    : out std_logic;

        -- NCO
        nco_freq_word  : out unsigned(23 downto 0);
        nco_freq_valid : out std_logic;
        
        -- from freq counter
        start_meas  : out std_logic;
        meas_done   : in std_logic;                            -- measurement done (pulse)
        start_tick  : in unsigned(REF_CNT_W-1 downto 0);
        end_tick    : in unsigned(REF_CNT_W-1 downto 0);
        N_counted   : in unsigned(31 downto 0)                 
        
        
        --from FIFO
        
        --from status register
        
    );
end entity;

architecture rtl of spi_decode  is

----------------------------------------------------------------
-- FIFO
----------------------------------------------------------------
constant FIFO_DEPTH_PER_VAR : integer := 8;
constant FIFO_DEPTH : integer := 8*4;

constant FPGA_COUNTER_PTR_START : integer :=0;
constant DELTA_TICK_PTR_START : integer :=8;
constant N_COUNTED_PTR_START : integer :=24;

type fifo_array is array (0 to FIFO_DEPTH-1) of std_logic_vector(31 downto 0);

signal fifo_mem    : fifo_array := (
    x"00000000", x"00000000", x"00000000", x"00000000", x"00000000", x"00000000", x"00000000", x"00000000", --test data
    x"00000000", x"00000000", x"00000000", x"00000000", x"00000000", x"00000000", x"00000000", x"00000000",--end_tick - start_tick (64bits words)
    x"00000000", x"00000000", x"00000000", x"00000000", x"00000000", x"00000000", x"00000000", x"00000000",
    x"00000000", x"00000000", x"00000000", x"00000000", x"00000000", x"00000000", x"00000000", x"00000000"--N_counted
);

signal fifo_rd_ptr : integer range 0 to FIFO_DEPTH-1 := 0;
signal fifo_rd_ptr_var : integer range 0 to FIFO_DEPTH-1 := 0;
signal fifo_rd_ptr_incr : integer :=0;

signal fifo_wr_ptr_var : integer range 0 to FIFO_DEPTH_PER_VAR-1 := 0;
signal fifo_fpga_counter_wr_ptr : integer range FPGA_COUNTER_PTR_START to (DELTA_TICK_PTR_START-1) := FPGA_COUNTER_PTR_START;
signal fifo_delta_tick_wr_ptr : integer range DELTA_TICK_PTR_START to (N_COUNTED_PTR_START-1) := DELTA_TICK_PTR_START;
signal fifo_N_counted_wr_ptr : integer range N_COUNTED_PTR_START to (FIFO_DEPTH-1) := N_COUNTED_PTR_START;

signal fifo_count  : integer range 0 to FIFO_DEPTH_PER_VAR := 0;
signal fifo_rd  : std_logic := '0';
signal fifo_full  : std_logic := '0';

-- measurement data in
signal delta_tick : unsigned(63 downto 0);
signal fifo_wr_count  : unsigned(31 downto 0) := (others => '0');

signal start_meas_r : std_logic :='0';
signal start_meas_d : std_logic :='0';
signal start_meas_status : std_logic :='0';

signal led_on_state : std_logic := '0';
signal led_off_state: std_logic :='0';
----------------------------------------------------------------
-- Status / control
----------------------------------------------------------------
signal start_status    : std_logic := '0';
signal start_read_data : std_logic := '0';
signal error_flag      : std_logic := '0';

signal status_reg : std_logic_vector(31 downto 0);

----------------------------------------------------------------
-- TX FSM
----------------------------------------------------------------
type tx_state_t is (TX_IDLE, TX_STATUS, TX_READ_DATA);
signal tx_state   : tx_state_t := TX_IDLE;
signal word_cnt   : integer range 0 to FIFO_DEPTH := 0;
signal nb_word   : integer range 0 to FIFO_DEPTH := 0;

begin

----------------------------------------------------------------
-- STATUS REGISTER
----------------------------------------------------------------
status_reg(31 downto 24) <= x"00"; -- MAJOR VERSION
status_reg(23 downto 16) <= x"03"; -- MINOR VERSION
status_reg(15 downto 8) <= std_logic_vector(to_unsigned(fifo_count, status_reg(15 downto 8)'length));
status_reg(7) <= start_meas_status;
status_reg(6) <= fifo_full;
status_reg(5) <= error_flag;
status_reg(4) <= nco_freq_valid;
status_reg(3) <= led_on;
status_reg(2) <= led_off;
status_reg(1) <= led_toggle;
status_reg(0) <= '0';

process(clk, reset_n)
begin
  if reset_n = '0' then
    led_on_state <= '0';
    led_off_state <= '0';
  elsif rising_edge(clk) then
    if led_on = '1' then
      led_on_state <= '1';
      led_off_state <='0';
    elsif led_off = '1' then
      led_on_state <= '0';
      led_off_state <='1';
    end if;
  end if;
end process;

process(clk)
begin
  if reset_n = '0' then
    start_meas_status <= '0';
  elsif rising_edge(clk) then    
    if start_meas ='1' then
       start_meas_status <= '1';
    elsif meas_done = '1' then
       start_meas_status <= '0';
    end if;
  end if;
end process;

----------------------------------------------------------------
-- FIFO write
----------------------------------------------------------------
--remplir fifo_array avec start_tick end_tick, N_counter <= fifo_mem(fifo_rd_ptr);
delta_tick <= end_tick - start_tick;
process(clk)
begin
  if rising_edge(clk) then
    start_meas_d <= '0';
    if meas_done = '1' and fifo_full = '0' then
        fifo_fpga_counter_wr_ptr <= (fifo_fpga_counter_wr_ptr + 1) mod (FIFO_DEPTH_PER_VAR);
        fifo_delta_tick_wr_ptr <= ((fifo_delta_tick_wr_ptr - 8 + 2) mod (2*FIFO_DEPTH_PER_VAR)) + DELTA_TICK_PTR_START;
        fifo_N_counted_wr_ptr <= (fifo_N_counted_wr_ptr + 1) mod (FIFO_DEPTH_PER_VAR) + N_COUNTED_PTR_START;
        
        fifo_wr_count  <= fifo_wr_count + 1;
        fifo_mem(fifo_fpga_counter_wr_ptr) <= std_logic_vector(fifo_wr_count);
        fifo_mem(fifo_delta_tick_wr_ptr) <= std_logic_vector(delta_tick(31 downto 0));
        fifo_mem(fifo_delta_tick_wr_ptr+1) <= std_logic_vector(delta_tick(63 downto 32));
        fifo_mem(fifo_N_counted_wr_ptr) <= std_logic_vector(N_counted);
        start_meas_d <= '1'; --pulse start new measurement
     end if;
  end if;
end process;

process(clk)
begin
  --if reset_n = '0' then
  --  fifo_wr_ptr <= (others => '0');
  if rising_edge(clk) then
    if fifo_rd='1' and (fifo_count > 0) then
        fifo_count <= fifo_count - 1;
    elsif meas_done = '1' and fifo_full = '0' then
        fifo_count <= fifo_count + 1;
    end if;
  end if;
end process;


----------------------------------------------------------------
-- COMMAND DECODE
----------------------------------------------------------------
process(clk, reset_n)
begin
    if reset_n = '0' then
        start_status     <= '0';
        start_read_data  <= '0';
        led_on           <= '0';
        led_off          <= '0';
        led_toggle       <= '0';
        nco_freq_word    <= (others => '0');
        nco_freq_valid   <= '0';
        error_flag       <= '0';

    elsif rising_edge(clk) then
       -- par défaut, signaux impulsionnels
       led_on        <= '0';
       led_off       <= '0';
       led_toggle    <= '0';
       start_status    <= '0';
       start_read_data <= '0';
       error_flag      <= '0';
       nco_freq_valid  <= '0';
       start_meas_r <= '0';
        
        if rx_valid = '1' then
            case rx_word(31 downto 24) is
                when x"00" =>
                    null;
                    
                -- READ STATUS
                when x"01" =>
                    start_status <= '1';

                -- CONTROL
                when x"02" =>
                    case rx_word(23 downto 16) is
                        when x"01" => led_on     <= '1';
                        when x"02" => led_off    <= '1';
                        when x"03" => led_toggle <= '1';
                        when x"04" =>
                            if start_meas_status = '1' then
                                 error_flag <= '1';
                            else 
                                start_meas_r <= '1';
                            end if;
                        when others => error_flag <= '1';
                    end case;

                -- NCO
                when x"04" =>
                    nco_freq_word  <= unsigned(rx_word(23 downto 0));
                    nco_freq_valid <= '1';

                -- READ FIFO DATA
                when x"10" =>
                    if to_integer(unsigned(rx_word(7 downto 0))) > FIFO_DEPTH then
                        error_flag <= '1';
                        start_read_data <= '0';
                    else
                        start_read_data <= '1';
                        case rx_word(23 downto 16) is
                            when x"00" => 
                                fifo_rd_ptr_var <=0;
                                nb_word <= to_integer(unsigned(rx_word(7 downto 0)));
                            when x"01" => 
                                fifo_rd_ptr_var <= DELTA_TICK_PTR_START;
                                nb_word <= 2*(to_integer(unsigned(rx_word(7 downto 0))));
                            when x"02" => 
                                fifo_rd_ptr_var <= N_COUNTED_PTR_START;
                                nb_word <= to_integer(unsigned(rx_word(7 downto 0)));
                            when others => error_flag <= '1';
                         end case;
                    end if;
                when others =>
                    error_flag <= '1';
            end case;
        end if;
    end if;
end process;

----------------------------------------------------------------
-- TX FSM (LOGICAL, PHY-DRIVEN)
----------------------------------------------------------------
process(clk, reset_n)
begin
    if reset_n = '0' then
        tx_state <= TX_IDLE;
        tx_word  <= (others => '0');
        tx_load  <= '0';
        word_cnt <= 0;

    elsif rising_edge(clk) then
        tx_load <= '0';
        fifo_rd  <= '0';

        case tx_state is

            ----------------------------------------------------
            when TX_IDLE =>
                if start_status = '1' and tx_busy = '0' then
                    tx_word <= status_reg;
                    tx_load <= '1';
                    tx_state <= TX_STATUS;

                elsif start_read_data = '1' and tx_busy = '0' then
                    fifo_rd_ptr <= fifo_rd_ptr_var;
                    tx_word <= fifo_mem(fifo_rd_ptr_var);
                    tx_load <= '1';
                    word_cnt <= nb_word;
                    --fifo_rd_count <= fifo_count_start;
                    tx_state <= TX_READ_DATA;
                    fifo_rd  <= '1';
                end if;

            ----------------------------------------------------
            when TX_STATUS =>
                if tx_busy = '0' then
                    tx_state <= TX_IDLE;
                end if;

            ----------------------------------------------------
            when TX_READ_DATA =>
                if tx_busy = '0' and tx_load ='0' then
                    if word_cnt > 0 and  fifo_rd_ptr < FIFO_DEPTH-1 then
                        fifo_rd_ptr <= (fifo_rd_ptr + 1);
                        fifo_rd  <= '1';
                        tx_word     <= fifo_mem((fifo_rd_ptr + 1));
                        tx_load     <= '1';
                        word_cnt    <= word_cnt - 1;
                    else
                        tx_state <= TX_IDLE;
                    end if;
                end if;

        end case;
    end if;
end process;
fifo_full <= '1' when (fifo_count = FIFO_DEPTH_PER_VAR) else '0';
start_meas <= start_meas_r or start_meas_d;
end architecture;