library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

entity top_reciproc_freq_meas is
    Port (
        clk_master : in  std_logic;  -- 12 MHz
        reset_n    : in  std_logic;
        sck        : in  std_logic;
        mosi       : in  std_logic;
        cs_n       : in  std_logic;
        LED0       : out std_logic
    );
end entity top_reciproc_freq_meas;

architecture rtl of top_reciproc_freq_meas is

    -- Signal interne du SPI slave
    signal led_spi : std_logic;

    -- (Futurs signaux NCO)
    signal nco_out : std_logic := '0';

begin

    -------------------------------------------------------------------
    -- Instance du module SPI Slave
    -------------------------------------------------------------------
    spi_slave_inst : entity work.spi_slave
        port map (
            clk_master => clk_master,
            reset_n    => reset_n,
            sck        => sck,
            mosi       => mosi,
            cs_n       => cs_n,
            LED0       => led_spi
        );

    -------------------------------------------------------------------
    -- (Futur) NCO : à instancier ici plus tard
    -------------------------------------------------------------------
    -- nco_inst : entity work.nco
    --     port map (
    --         clk => clk_master,
    --         reset_n => reset_n,
    --         freq_word => ...,
    --         nco_out => nco_out
    --     );

    -------------------------------------------------------------------
    -- Sorties physiques
    -------------------------------------------------------------------
    LED0 <= led_spi;  -- pour le moment, directement la LED du SPI slave

end architecture rtl;
