library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

entity spi_slave is
    Port (
        sck    : in  std_logic;       -- Horloge SPI
        mosi   : in  std_logic;       -- Donnée maître -> esclave
        ce     : in  std_logic;       -- Chip Select actif bas
        LED0   : out std_logic        -- LED user sur Cmod A7
    );
end spi_slave;

architecture Behavioral of spi_slave is
    signal bit_cnt   : integer range 0 to 15 := 0;
    signal shift_reg : std_logic_vector(15 downto 0) := (others => '0');
    signal data_reg  : std_logic_vector(15 downto 0) := (others => '0');
    signal led_reg   : std_logic := '0';
    signal stop      : std_logic := '0';
begin

    process(sck, ce)
    begin
        if stop = '0' then
            if ce = '1' then
                if rising_edge(sck) then
                    shift_reg <= shift_reg(14 downto 0) & mosi;
                    if bit_cnt = 15 then
                        data_reg <= shift_reg(14 downto 0) & mosi;
                        bit_cnt <= 0;
                    else
                        bit_cnt <= bit_cnt + 1;
                    end if;
                end if;
            else
                bit_cnt <= 0;
            end if;
        end if;
    end process;

    -- Décodage des commandes reçues
    process(data_reg)
    begin
        if data_reg = x"FDEC" then
            led_reg <= not led_reg; -- Toggle LED
        elsif data_reg = x"FFFF" then
            stop <= '1'; -- Arrêt du module
        end if;
    end process;

    LED0 <= led_reg;

end Behavioral;
