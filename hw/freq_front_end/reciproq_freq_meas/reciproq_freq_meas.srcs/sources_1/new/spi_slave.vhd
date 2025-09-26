library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

entity spi_slave is
    Port (
        clk_master : in  std_logic;    -- 12 MHz interne
        reset_n    : in  std_logic;
        sck        : in  std_logic;    -- clock série du maître
        mosi       : in  std_logic;    -- données maître -> esclave
        cs_n       : in  std_logic;    -- chip select actif bas
        LED0       : out std_logic
    );
end spi_slave;

architecture rtl of spi_slave is
    -- synchronisation
    signal sck_sync  : std_logic_vector(2 downto 0) := (others => '0');
    signal mosi_sync : std_logic_vector(2 downto 0) := (others => '0');

    signal sck_rise  : std_logic;

    signal bit_count : integer range 0 to 31 := 0;
    signal shift_reg : std_logic_vector(31 downto 0) := (others => '0');

    signal led_reg   : std_logic := '0';
begin
    LED0 <= led_reg;

    -- synchronisation sur clk_master
    process(clk_master)
    begin
        if rising_edge(clk_master) then
            sck_sync  <= sck_sync(1 downto 0) & sck;
            mosi_sync <= mosi_sync(1 downto 0) & mosi;
        end if;
    end process;

    -- détection front montant SCK
    sck_rise <= '1' when (sck_sync(2 downto 1) = "01") else '0';

    -- réception série
    process(clk_master, reset_n)
    begin
    if reset_n = '0' then
        bit_count <= 0;
        shift_reg <= (others => '0');
        led_reg   <= '0';
    elsif rising_edge(clk_master) then
        if cs_n = '0' then
            if sck_rise = '1' then
                -- on insère directement le bit reçu
                shift_reg <= shift_reg(30 downto 0) & mosi_sync(2);

                if bit_count = 31 then
                    bit_count <= 0;
                    -- ⚠ comparaison sur la valeur complète (y compris le dernier bit)
                    if (shift_reg(30 downto 0) & mosi_sync(2)) = x"0000FDEC" then
                        led_reg <= not led_reg;
                    end if;
                else
                    bit_count <= bit_count + 1;
                end if;
            end if;
        else
            bit_count <= 0; -- reset si CS désactivé
            shift_reg <= (others => '0');
        end if;
    end if;
end process;
end rtl;
