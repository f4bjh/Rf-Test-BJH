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
                
        data_out   : out std_logic_vector(31 downto 0); -- mot reçu
        data_valid : out std_logic                      -- impulsion 1 clk_master à la fin du mot
    );
end spi_slave;

architecture rtl of spi_slave is
    -- synchronisation
    signal sck_sync  : std_logic_vector(2 downto 0) := (others => '0');
    signal mosi_sync : std_logic_vector(2 downto 0) := (others => '0');

    signal sck_rise  : std_logic;

    signal bit_count : integer range 0 to 31 := 0;
    signal shift_reg : std_logic_vector(31 downto 0) := (others => '0');

    signal data_out_reg   : std_logic_vector(31 downto 0) := (others => '0');
    signal data_valid_reg : std_logic := '0';
begin
    data_out   <= data_out_reg;
    data_valid <= data_valid_reg;

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
            bit_count      <= 0;
            shift_reg      <= (others => '0');
            data_out_reg   <= (others => '0');
            data_valid_reg <= '0';
        elsif rising_edge(clk_master) then
            -- réinitialisation par défaut
            data_valid_reg <= '0';
    
            -- synchronisation du SCK et échantillonnage sur front montant
            if cs_n = '0' and sck_sync(2 downto 1) = "01" then
                -- shift MSB first
                shift_reg <= shift_reg(30 downto 0) & mosi_sync(2);
    
                if bit_count = 31 then
                    bit_count      <= 0;
                    data_out_reg   <= shift_reg(30 downto 0) & mosi_sync(2);
                    data_valid_reg <= '1';
                else
                    bit_count <= bit_count + 1;
                end if;
            elsif cs_n = '1' then
                bit_count <= 0;
                shift_reg <= (others => '0');
            end if;
        end if;
    end process;

end rtl;
