library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

entity tb_spi_slave is
end tb_spi_slave;

architecture sim of tb_spi_slave is
    signal SCK   : std_logic := '0';
    signal MOSI  : std_logic := '0';
    signal CS_N  : std_logic := '1';
    signal LED0  : std_logic;

    constant CLK_PERIOD : time := 100 ns;

    -- Procedure pour envoyer 16 bits via SPI (mode 0)
    procedure spi_send(signal SCK : out std_logic;
                       signal MOSI : out std_logic;
                       signal CS_N : out std_logic;
                       data : std_logic_vector(15 downto 0)) is
    begin
        CS_N <= '0';
        for i in 15 downto 0 loop
            MOSI <= data(i);
            SCK <= '0'; wait for CLK_PERIOD/2;
            SCK <= '1'; wait for CLK_PERIOD/2;
        end loop;
        CS_N <= '1';
        wait for CLK_PERIOD;
    end procedure;

begin
    -- Instanciation du DUT
    uut: entity work.spi_slave
        port map (
            SCK  => SCK,
            MOSI => MOSI,
            CS_N => CS_N,
            LED0 => LED0
        );

    -- Stimulus
    stim_proc: process
    begin
        wait for 200 ns;

        -- Envoi FDEC
        spi_send(SCK, MOSI, CS_N, x"FDEC");
        wait for 500 ns;

        -- Envoi FDEC encore (la LED toggle)
        spi_send(SCK, MOSI, CS_N, x"FDEC");
        wait for 500 ns;

        -- Envoi FFFF (arrêt)
        spi_send(SCK, MOSI, CS_N, x"FFFF");
        wait for 500 ns;

        wait;
    end process;

end sim;
