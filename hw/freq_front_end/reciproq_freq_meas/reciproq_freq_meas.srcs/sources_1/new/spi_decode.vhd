library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity spi_decode is
    port (
        clk             : in  std_logic;
        reset_n         : in  std_logic;
        data_in         : in  std_logic_vector(31 downto 0);
        data_valid      : in  std_logic;
        led_on          : out std_logic;
        led_toggle      : out std_logic;
        led_off         : out std_logic;
        nco_freq_word   : out unsigned(23 downto 0);
        nco_freq_valid  : out std_logic
    );
end entity;

architecture rtl of spi_decode is
    signal cmd     : unsigned(7 downto 0);
    signal payload : unsigned(23 downto 0);
begin
    process(clk, reset_n)
    begin
        if reset_n = '0' then
            cmd           <= (others => '0');
            payload       <= (others => '0');
            led_on        <= '0';
            led_off       <= '0';
            led_toggle    <= '0';
            nco_freq_word <= (others => '0');
            nco_freq_valid<= '0';
        elsif rising_edge(clk) then
            -- par défaut, signaux impulsionnels
            led_on        <= '0';
            led_off       <= '0';
            led_toggle    <= '0';
            nco_freq_valid<= '0';

            if data_valid = '1' then
                cmd     <= unsigned(data_in(31 downto 24));
                payload <= unsigned(data_in(23 downto 0));

                case data_in(31 downto 24) is
                    when x"01" => led_on     <= '1';
                    when x"02" => led_off    <= '1';
                    when x"03" => led_toggle <= '1';
                    when x"04" =>
                        nco_freq_word  <= unsigned(data_in(23 downto 0));
                        nco_freq_valid <= '1';
                    when others => null;
                end case;
            end if;
        end if;
    end process;
end architecture;
