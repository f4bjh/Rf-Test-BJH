library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

entity tb_spi_slave is
end tb_spi_slave;

architecture sim of tb_spi_slave is
    signal clk_master : std_logic := '0';
    signal reset_n    : std_logic := '0';
    signal sck        : std_logic := '0';
    signal mosi       : std_logic := '0';
    signal cs_n       : std_logic := '1';
    signal LED0       : std_logic;

    constant CLK_PERIOD : time := 83.33 ns; -- 12 MHz

    -- Procédure utilitaire : envoie un mot 32 bits MSB first
    procedure send_word(signal cs_n   : out std_logic;
                        signal sck    : out std_logic;
                        signal mosi   : out std_logic;
                        word          : std_logic_vector(31 downto 0)) is
    begin
        cs_n <= '0';
        wait for 10*CLK_PERIOD;
        for i in 31 downto 0 loop
            mosi <= word(i);
            wait for 2 us;
            sck <= '1';
            wait for 2 us;
            sck <= '0';
        end loop;
        wait for 10*CLK_PERIOD;
        cs_n <= '1';
    end procedure;
begin
    -- horloge maître
    clk_master <= not clk_master after CLK_PERIOD/2;

    -- DUT
    uut: entity work.spi_slave
        port map (
            clk_master => clk_master,
            reset_n    => reset_n,
            sck        => sck,
            mosi       => mosi,
            cs_n       => cs_n,
            LED0       => LED0
        );

    -- Stimulus
    process
    begin
        -- Reset
        reset_n <= '0';
        wait for 500 ns;
        reset_n <= '1';
        wait for 500 ns;

        -- Transmission de 0x0000FDEC
        send_word(cs_n, sck, mosi, x"0000FDEC");
        wait for 10 ms;

        -- Transmission d'un autre mot
        send_word(cs_n, sck, mosi, x"12345678");
        wait for 10 ms;

        -- Re-transmission de 0x0000FDEC
        send_word(cs_n, sck, mosi, x"0000FDEC");
        wait for 10 ms;

        -- Re-transmission de 0x0000FDEC
        send_word(cs_n, sck, mosi, x"0000FDEC");
        wait for 10 ms;

        -- Re-transmission de 0x0000FDEC
        send_word(cs_n, sck, mosi, x"0000FDEC");
        wait for 10 ms;

        -- Re-transmission de 0x0000FDEC
        send_word(cs_n, sck, mosi, x"0000FDEC");
        wait for 10 ms;
        
        -- Re-transmission de 0x0000FDEC
        send_word(cs_n, sck, mosi, x"0000FDEC");
        wait for 10 ms;

        -- Re-transmission de 0x0000FDEC
        send_word(cs_n, sck, mosi, x"0000FDEC");
        wait for 10 ms;
        
        -- Re-transmission de 0x0000FDEC
        send_word(cs_n, sck, mosi, x"0000FDEC");
        wait for 10 ms;

        -- Re-transmission de 0x0000FDEC
        send_word(cs_n, sck, mosi, x"0000FDEC");
        wait for 10 ms;
        
        -- Re-transmission de 0x0000FDEC
        send_word(cs_n, sck, mosi, x"0000FDEC");
        wait for 10 ms;
        
    end process;
end sim;
