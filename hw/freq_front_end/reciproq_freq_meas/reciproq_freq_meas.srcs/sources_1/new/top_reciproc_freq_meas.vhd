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
        LED0       : out std_logic;
        NCO_OUT    : out std_logic
    );
end entity top_reciproc_freq_meas;

architecture rtl of top_reciproc_freq_meas is

    -- Signal interne du SPI slave
    signal led_spi : std_logic;

    -- Signal interne du NCO
    signal nco_clk_out : std_logic;

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
    -- Instance du NCO (DDS simple)
    -------------------------------------------------------------------
    nco_inst : entity work.nco
        generic map (
            clk_freq => 12_000_000.0,  -- fréquence de l'horloge maître
            freq     => 100_000.0      -- fréquence de sortie (100 kHz)
        )
        port map (
            clk_in  => clk_master,
            reset_n => reset_n,
            clk_out => nco_clk_out
        );

    -------------------------------------------------------------------
    -- Sorties physiques
    -------------------------------------------------------------------
    LED0   <= led_spi;
    NCO_OUT <= nco_clk_out;

end architecture rtl;
