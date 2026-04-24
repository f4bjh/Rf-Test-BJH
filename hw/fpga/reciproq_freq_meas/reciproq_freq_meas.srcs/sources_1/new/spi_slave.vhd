library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity spi_slave is
    port (
        clk     : in  std_logic;
        reset_n : in  std_logic;

        -- SPI pins
        sclk    : in  std_logic;
        cs_n    : in  std_logic;
        mosi    : in  std_logic;
        miso    : out std_logic;

        -- RX interface
        rx_word  : out std_logic_vector(31 downto 0);
        rx_valid : out std_logic;

        -- TX interface
        tx_word  : in  std_logic_vector(31 downto 0);
        tx_load  : in  std_logic;
        tx_busy  : out std_logic
    );
end entity;

architecture rtl of spi_slave is

    signal sclk_d, sclk_dd : std_logic := '0';
    signal sclk_rise       : std_logic;
    signal sclk_fall       : std_logic;

    -- RX
    signal rx_shift : std_logic_vector(31 downto 0) := (others => '0');
    signal rx_cnt   : integer range 0 to 31 := 0;

    -- TX
    signal tx_shift  : std_logic_vector(31 downto 0) := (others => '0');
    signal tx_cnt    : integer range 0 to 32 := 0;
    signal tx_active : std_logic := '0';
    signal miso_reg  : std_logic := '0';

begin

----------------------------------------------------------------
-- SCLK sync
----------------------------------------------------------------
process(clk)
begin
    if rising_edge(clk) then
        sclk_d  <= sclk;
        sclk_dd <= sclk_d;
    end if;
end process;

sclk_rise <= sclk_d  and not sclk_dd;
sclk_fall <= not sclk_d and sclk_dd;

----------------------------------------------------------------
-- RX SHIFT (unchanged)
----------------------------------------------------------------
process(clk)
begin
    if rising_edge(clk) then
        rx_valid <= '0';

        if cs_n = '1' then
            rx_cnt <= 0;

        elsif sclk_rise = '1' then
            rx_shift <= rx_shift(30 downto 0) & mosi;

            if rx_cnt = 31 then
                rx_word  <= rx_shift(30 downto 0) & mosi;
                rx_valid <= '1';
                rx_cnt   <= 0;
            else
                rx_cnt <= rx_cnt + 1;
            end if;
        end if;
    end if;
end process;

----------------------------------------------------------------
-- TX SHIFT (CORRECT CPHA ALIGNMENT)
----------------------------------------------------------------
process(clk)
begin
    if rising_edge(clk) then

        if tx_load = '1' then
            tx_shift  <= tx_word;
            tx_cnt    <= 32;
            tx_active <= '1';

        elsif tx_active = '1' and cs_n = '0' and sclk_fall = '1' then
            -- prepare next bit BEFORE master samples
            miso_reg <= tx_shift(31);
            tx_shift <= tx_shift(30 downto 0) & '0';
            tx_cnt   <= tx_cnt - 1;

            if tx_cnt = 1 then
                tx_active <= '0';
            end if;
        end if;

        if cs_n = '1' then
            tx_active <= '0';
        end if;
    end if;
end process;

----------------------------------------------------------------
-- Outputs
----------------------------------------------------------------
miso    <= miso_reg;
tx_busy <= tx_active;

end architecture;
