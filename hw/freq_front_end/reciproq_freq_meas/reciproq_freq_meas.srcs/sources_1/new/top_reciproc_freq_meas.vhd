library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity top_reciproc_freq_meas is
    port(
        clk_master : in  std_logic;
        reset_n    : in  std_logic;
        sck        : in  std_logic;
        mosi       : in  std_logic;
        cs_n       : in  std_logic;
        LED0       : out std_logic;
        NCO_OUT    : out std_logic
    );
end entity;

architecture rtl of top_reciproc_freq_meas is
    -- SPI slave interface
    signal spi_word      : std_logic_vector(31 downto 0);
    signal spi_data_valid: std_logic;

    -- LED signals
    signal led_state : std_logic := '0';
    signal led_on, led_off, led_toggle : std_logic;

    -- NCO
    signal freq_word      : unsigned(23 downto 0);
    signal freq_valid     : std_logic;
    signal nco_clk_out    : std_logic;
begin
    -- SPI Slave
    spi_slave_inst : entity work.spi_slave
        port map(
            clk_master => clk_master,
            reset_n    => reset_n,
            sck        => sck,
            mosi       => mosi,
            cs_n       => cs_n,
            data_out   => spi_word,
            data_valid => spi_data_valid
        );

    -- SPI Decoder
    spi_decode_inst : entity work.spi_decode
        port map(
            clk            => clk_master,
            reset_n        => reset_n,
            data_in        => spi_word,
            data_valid     => spi_data_valid,
            led_on         => led_on,
            led_off        => led_off,
            led_toggle     => led_toggle,
            nco_freq_word  => freq_word,
            nco_freq_valid => freq_valid
        );
        
    -- NCO
    nco_inst : entity work.nco
       port map (
           clk_in     => clk_master,
           reset_n    => reset_n,
           freq_word  => freq_word,
           freq_valid => freq_valid,
           clk_out    => NCO_OUT
       );
       
    -- LED management
    process(clk_master, reset_n)
    begin
        if reset_n = '0' then
            led_state <= '0';
        elsif rising_edge(clk_master) then
            if led_on = '1' then
                led_state <= '1';
            elsif led_off = '1' then
                led_state <= '0';
            elsif led_toggle = '1' then
                led_state <= not led_state;
            end if;
        end if;
    end process;

    LED0 <= led_state;

end architecture;
