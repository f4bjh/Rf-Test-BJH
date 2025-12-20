library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity spi_slave is
    port (
        clk         : in  std_logic;
        reset_n     : in  std_logic;

        -- SPI
        sclk        : in  std_logic;
        cs_n        : in  std_logic;
        mosi        : in  std_logic;
        miso        : out std_logic;

        -- LED
        led_on          : out std_logic;
        led_toggle      : out std_logic;
        led_off         : out std_logic;
 
        -- NCO
        nco_freq_word  : out unsigned(23 downto 0);
        nco_freq_valid : out std_logic
    );
end entity;

architecture rtl of spi_slave  is

----------------------------------------------------------------
-- SPI clock sync
----------------------------------------------------------------
signal sclk_d    : std_logic := '0';
signal sclk_rise : std_logic := '0';

----------------------------------------------------------------
-- RX
----------------------------------------------------------------
signal rx_shift   : std_logic_vector(31 downto 0) := (others => '0');
signal rx_bit_cnt : integer range 0 to 31 := 0;
signal rx_word    : std_logic_vector(31 downto 0) := (others => '0');
signal data_valid : std_logic := '0';

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
signal fifo_dout   : std_logic_vector(31 downto 0);

----------------------------------------------------------------
-- TX / FSM
----------------------------------------------------------------
type tx_state_t is (TX_IDLE, TX_LOAD, TX_STATUS, TX_READ_DATA);
signal tx_state   : tx_state_t := TX_IDLE;

signal tx_shift   : std_logic_vector(31 downto 0) := (others => '0');
signal tx_bit_cnt : integer range 0 to 31 := 31;
signal word_cnt   : integer range 0 to 7 := 0;

signal start_read_data : std_logic := '0';
signal start_status    : std_logic := '0';
signal status_reg  : std_logic_vector(31 downto 0):= (others => '0');
signal error_flag  : std_logic:='0';


begin

---------------------------------------------------------------
-- STATUS REGISTER
----------------------------------------------------------------
status_reg(31 downto 24) <= x"01"; -- VERSION
status_reg(7) <= '0';
status_reg(6) <= '0';
status_reg(5) <= error_flag;
status_reg(4) <= nco_freq_valid;
status_reg(3) <= led_on;
status_reg(2) <= led_off;
status_reg(1) <= led_toggle;
status_reg(0) <= '0';

----------------------------------------------------------------
-- SCLK edge detect
----------------------------------------------------------------
process(clk)
begin
    if rising_edge(clk) then
        sclk_d <= sclk;
    end if;
end process;

sclk_rise <= sclk and not sclk_d;

----------------------------------------------------------------
-- SPI RX (32 bits command)
----------------------------------------------------------------
process(clk)
begin
    if rising_edge(clk) then
        if cs_n = '1' then
            rx_bit_cnt <= 0;
            data_valid <= '0';

        elsif sclk_rise = '1' then
            rx_shift <= rx_shift(30 downto 0) & mosi;

            if rx_bit_cnt = 31 then
                rx_word    <= rx_shift(30 downto 0) & mosi;
                data_valid <= '1';
                rx_bit_cnt <= 0;
            else
                rx_bit_cnt <= rx_bit_cnt + 1;
                data_valid <= '0';
            end if;
        end if;
    end if;
end process;

----------------------------------------------------------------
-- COMMAND DECODE (flags only)
----------------------------------------------------------------
process(clk, reset_n)
begin
    if reset_n = '0' then
        start_read_data <= '0';
        start_status    <= '0';
        led_on        <= '0';
        led_off       <= '0';
        led_toggle    <= '0';
        nco_freq_word   <= (others => '0');
        nco_freq_valid  <= '0';
        error_flag      <= '0';

    elsif rising_edge(clk) then
        start_read_data <= '0';
        start_status    <= '0';
        error_flag <= '0';

        if data_valid = '1' then
            case rx_word(31 downto 24) is
            
                --read status
                when x"01" => start_status    <= '1';
                
                -- led
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
 
                -- read data
                when x"10" => start_read_data <= '1';
                
                when others => error_flag <= '1';
            end case;
        end if;
    end if;
end process;

----------------------------------------------------------------
-- FIFO READ (simple)
----------------------------------------------------------------
process(clk)
begin
    if rising_edge(clk) then
        if fifo_count > 0 then
            fifo_dout <= fifo_mem(fifo_rd_ptr);
        end if;
    end if;
end process;

----------------------------------------------------------------
-- TX FSM (UNIQUE DRIVER)
----------------------------------------------------------------
process(clk, reset_n)
begin
    if reset_n = '0' then
        tx_state   <= TX_IDLE;
        tx_shift   <= (others => '0');
        tx_bit_cnt <= 31;
        word_cnt   <= 0;

    elsif rising_edge(clk) then

        case tx_state is

            ----------------------------------------------------
            when TX_IDLE =>
                if start_status = '1' then
                    tx_shift   <= status_reg;
                    tx_bit_cnt <= 31;
                    tx_state   <= TX_LOAD;

                elsif start_read_data = '1' and fifo_count > 0 then
                    tx_shift   <= fifo_dout;
                    tx_bit_cnt <= 31;
                    word_cnt   <= 0;
                    tx_state   <= TX_LOAD;
                end if;

            ----------------------------------------------------
            when TX_LOAD =>
                if start_status = '0' and start_read_data = '0' then
                   if tx_shift = status_reg then
                      tx_state <= TX_STATUS;
                   else
                      tx_state <= TX_READ_DATA;
                   end if;
                end if;                

            ----------------------------------------------------
            when TX_STATUS =>
                if sclk_rise = '1' then
                    if tx_bit_cnt > 0 then
                        tx_shift   <= tx_shift(30 downto 0) & '0';
                        tx_bit_cnt <= tx_bit_cnt - 1;
                    else
                        tx_state <= TX_IDLE;
                    end if;
                end if;

            ----------------------------------------------------
            when TX_READ_DATA =>
                if sclk_rise = '1' then
                    if tx_bit_cnt > 0 then
                        tx_shift   <= tx_shift(30 downto 0) & '0';
                        tx_bit_cnt <= tx_bit_cnt - 1;
                    else
                        if word_cnt < 7 then
                            fifo_rd_ptr <= (fifo_rd_ptr + 1) mod FIFO_DEPTH;
                            fifo_count  <= fifo_count - 1;
                            tx_shift   <= fifo_mem((fifo_rd_ptr + 1) mod FIFO_DEPTH);
                            tx_bit_cnt <= 31;
                            word_cnt   <= word_cnt + 1;
                        else
                            tx_state <= TX_IDLE;
                        end if;
                    end if;
                end if;
        end case;
    end if;
end process;

miso <= tx_shift(31);

end architecture;