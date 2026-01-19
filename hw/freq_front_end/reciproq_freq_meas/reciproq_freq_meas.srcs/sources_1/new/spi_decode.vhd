library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity spi_decode is
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
        nco_freq_valid : out std_logic
    );
end entity;

architecture rtl of spi_decode  is

----------------------------------------------------------------
-- FIFO
----------------------------------------------------------------
constant FIFO_DEPTH : integer := 8;
type fifo_array is array (0 to FIFO_DEPTH-1) of std_logic_vector(31 downto 0);

signal fifo_mem    : fifo_array := (
    x"00000000", x"00000001", x"00000002", x"00000003",
    x"00000004", x"00000005", x"00000006", x"00000007"
);

signal fifo_rd_ptr : integer range 0 to FIFO_DEPTH-1 := 0;
signal fifo_count  : integer range 0 to FIFO_DEPTH := FIFO_DEPTH;
signal fifo_full : std_logic := '0';

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

begin

----------------------------------------------------------------
-- STATUS REGISTER
----------------------------------------------------------------
status_reg(31 downto 24) <= x"01"; -- VERSION
status_reg(23 downto 8)  <= (others => '0');
status_reg(7) <= '0';
status_reg(6) <= fifo_full;
status_reg(5) <= error_flag;
status_reg(4) <= nco_freq_valid;
status_reg(3) <= led_on;
status_reg(2) <= led_off;
status_reg(1) <= led_toggle;
status_reg(0) <= '0';

----------------------------------------------------------------
-- FIFO READ (combinational read style)
----------------------------------------------------------------
--fifo_dout <= fifo_mem(fifo_rd_ptr);

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
        


        if rx_valid = '1' then
            case rx_word(31 downto 24) is

                -- READ STATUS
                when x"01" =>
                    start_status <= '1';

                -- LED CONTROL
                when x"02" =>
                    case rx_word(23 downto 16) is
                        when x"01" => led_on     <= '1';
                        when x"02" => led_off    <= '1';
                        when x"03" => led_toggle <= '1';
                        when others => error_flag <= '1';
                    end case;

                -- NCO
                when x"04" =>
                    nco_freq_word  <= unsigned(rx_word(23 downto 0));
                    nco_freq_valid <= '1';

                -- READ FIFO DATA
                when x"10" =>
                    start_read_data <= '1';

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

        case tx_state is

            ----------------------------------------------------
            when TX_IDLE =>
                if start_status = '1' and tx_busy = '0' then
                    tx_word <= status_reg;
                    tx_load <= '1';
                    tx_state <= TX_STATUS;

                elsif start_read_data = '1' and fifo_count > 0 and tx_busy = '0' then
                    tx_word <= fifo_mem(fifo_rd_ptr);
                    tx_load <= '1';
                    word_cnt <= 0;
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
                    if word_cnt < FIFO_DEPTH-1 and fifo_count > 1 then
                        fifo_rd_ptr <= (fifo_rd_ptr + 1) mod FIFO_DEPTH;
                        fifo_count  <= fifo_count - 1;
                        tx_word     <= fifo_mem((fifo_rd_ptr + 1) mod FIFO_DEPTH);
                        tx_load     <= '1';
                        word_cnt    <= word_cnt + 1;
                    else
                        tx_state <= TX_IDLE;
                    end if;
                end if;

        end case;
    end if;
end process;
fifo_full <= '1' when (fifo_count = FIFO_DEPTH) else '0';
end architecture;