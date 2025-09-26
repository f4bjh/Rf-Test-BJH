library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

entity tb_spi_slave is
end tb_spi_slave;

architecture sim of tb_spi_slave is
    signal sck   : std_logic := '0';
    signal mosi  : std_logic := '0';
    signal ce  : std_logic := '0';
    signal LED0  : std_logic;

    constant CLK_PERIOD : time := 100 ns;

    -- Procedure pour envoyer 16 bits via SPI (mode 0)
    procedure spi_send(signal sck : out std_logic;
                       signal mosi : out std_logic;
                       signal ce : out std_logic;
                       data : std_logic_vector(15 downto 0)) is
    begin
        ce <= '1';
        for i in 15 downto 0 loop
            mosi <= data(i);
            sck <= '0'; wait for CLK_PERIOD/2;
            sck <= '1'; wait for CLK_PERIOD/2;
        end loop;
        ce <= '0';
        wait for CLK_PERIOD;
    end procedure;

begin
    -- Instanciation du DUT
    uut: entity work.spi_slave
        port map (
            sck  => sck,
            mosi => mosi,
            ce => ce,
            LED0 => LED0
        );

    -- Stimulus
    stim_proc: process
    begin
        wait for 200 ns;

        -- Envoi FDEC
        spi_send(sck, mosi, ce, x"FDEC");
        wait for 500 ns;

        -- Envoi FDEC encore (la LED toggle)
        spi_send(sck, mosi, ce, x"FDEC");
        wait for 500 ns;

        -- Envoi FFFF (arrêt)
        spi_send(sck, mosi, ce, x"FFFF");
        wait for 500 ns;

        wait;
    end process;

end sim;
