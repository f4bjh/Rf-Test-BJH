library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity spi_decode is
  generic (
    FREF_HZ     : natural := 12_000_000;
    REF_CNT_W   : natural := 64;
    MAX_N       : natural := 200_000;
    TIMEOUT_MS  : natural := 2000;
    FIFO_DEPTH_PER_VAR : integer := 8;
    FIFO_DEPTH         : integer := 32
  );
  port (
    clk     : in  std_logic;
    reset_n : in  std_logic;

    -- SPI RX
    rx_word  : in  std_logic_vector(31 downto 0);
    rx_valid : in  std_logic;

    -- SPI TX
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
    cfg_N          : out unsigned(31 downto 0);

    -- measurement
    start_meas  : out std_logic;
    meas_done   : in  std_logic;

    -- FIFO interface
    fifo_rd_data : in  std_logic_vector(31 downto 0);
    fifo_rd      : out std_logic;
    fifo_data_type_req  : out std_logic_vector(7 downto 0);
    fifo_full    : in  std_logic;
    fifo_wr   : out std_logic;

    -- STATUS
    error_flag      : out std_logic := '0';
    status_reg_in : in std_logic_vector(31 downto 0)
  );
end entity;

architecture rtl of spi_decode is

----------------------------------------------------------------
-- Control / status
----------------------------------------------------------------
signal start_status    : std_logic := '0';
signal start_read_data : std_logic := '0';

signal start_meas_r : std_logic := '0';

----------------------------------------------------------------
-- FIFO read control
----------------------------------------------------------------
signal fifo_rd_d : std_logic := '0';
signal fifo_rd_i : std_logic := '0';

----------------------------------------------------------------
-- TX FSM
----------------------------------------------------------------
type tx_state_t is (TX_IDLE, TX_STATUS, TX_READ_DATA);
signal tx_state   : tx_state_t := TX_IDLE;
signal word_cnt   : integer range 0 to FIFO_DEPTH := 0;
signal nb_word    : integer range 0 to FIFO_DEPTH := 0;

begin

----------------------------------------------------------------
-- FIFO WRITE CONTROL 
----------------------------------------------------------------
fifo_wr <= '1' when (meas_done = '1' and fifo_full = '0') else '0';

----------------------------------------------------------------
-- COMMAND DECODE
----------------------------------------------------------------
process(clk, reset_n)
begin
    if reset_n = '0' then
        start_status       <= '0';
        start_read_data    <= '0';
        led_on             <= '0';
        led_off            <= '0';
        led_toggle         <= '0';
        nco_freq_word      <= (others => '0');
        nco_freq_valid     <= '0';
        error_flag         <= '0';
        start_meas_r       <= '0';
        fifo_data_type_req <= x"00";
        fifo_rd_i          <= '0';
        cfg_N              <= to_unsigned(100, 32);
        
    elsif rising_edge(clk) then

        -- pulses
        led_on        <= '0';
        led_off       <= '0';
        led_toggle    <= '0';
        start_status    <= '0';
        start_read_data <= '0';
        error_flag      <= '0';
        nco_freq_valid  <= '0';
        start_meas_r    <= '0';
        fifo_rd_i  <= '0';

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
                            start_meas_r <= '1';
                        when others =>
                            error_flag <= '1';
                    end case;

                -- NCO
                when x"03" =>
                    case rx_word(23 downto 0) is
                        when x"000000" =>
                            cfg_N <= to_unsigned(100, 32);
                        when x"000001" =>
                            cfg_N <= to_unsigned(1000, 32);    
                        when x"000002" =>
                            cfg_N <= to_unsigned(10000, 32);
                        when x"000003" =>
                            cfg_N <= to_unsigned(100000, 32);
                        when x"000004" =>
                            cfg_N <= to_unsigned(1000000, 32);
                        when others =>
                            error_flag <= '1';
                    end case;
        
                when x"04" =>
                    nco_freq_word  <= unsigned(rx_word(23 downto 0));
                    nco_freq_valid <= '1';

                -- READ FIFO
                when x"10" =>
                    if to_integer(unsigned(rx_word(7 downto 0))) > FIFO_DEPTH then
                        error_flag <= '1';
                    elsif start_read_data = '0' then
                        start_read_data <= '1';
                        fifo_rd_i  <= '1';
                        case rx_word(23 downto 16) is
                            when x"00" =>
                                fifo_data_type_req <= x"00";
                                nb_word <= to_integer(unsigned(rx_word(7 downto 0)));

                            when x"01" =>
                                fifo_data_type_req <= x"01";
                                nb_word <= 2 * to_integer(unsigned(rx_word(7 downto 0)));

                            when x"02" =>
                                fifo_data_type_req <= x"02";
                                nb_word <= to_integer(unsigned(rx_word(7 downto 0)));

                            when others =>
                                error_flag <= '1';
                        end case;
                    end if;

                when others =>
                    error_flag <= '1';
            end case;
        end if;
    end if;
end process;

----------------------------------------------------------------
-- TX FSM
----------------------------------------------------------------
process(clk, reset_n)
begin
    if reset_n = '0' then
        tx_state <= TX_IDLE;
        tx_word  <= (others => '0');
        tx_load  <= '0';
        word_cnt <= 0;
        fifo_rd_d <= '0';
        
    elsif rising_edge(clk) then

        tx_load <= '0';
        fifo_rd_d <= '0';
        
        case tx_state is

            ----------------------------------------------------
            when TX_IDLE =>
                if start_status = '1' and tx_busy = '0' then
                    tx_word <= status_reg_in;
                    tx_load <= '1';
                    tx_state <= TX_STATUS;

                elsif start_read_data = '1' and tx_busy = '0' then
                    word_cnt <= nb_word;
                    tx_state <= TX_READ_DATA;
                end if;
            ----------------------------------------------------
            when TX_STATUS =>
                if tx_busy = '0' then
                    tx_state <= TX_IDLE;
                end if;
                
            ----------------------------------------------------
            when TX_READ_DATA =>
                 if tx_busy = '0' and tx_load ='0' then
                    if word_cnt > 0 then
                        fifo_rd_d  <= '1';
                        tx_word    <= fifo_rd_data;
                        tx_load    <= '1';
                        word_cnt   <= word_cnt - 1;
                    else
                        tx_state <= TX_IDLE;
                    end if;
                end if;
           end case;
    end if;
end process;

----------------------------------------------------------------
-- OUTPUT
----------------------------------------------------------------
start_meas <= start_meas_r;
fifo_rd <= fifo_rd_d or fifo_rd_i;

end architecture;