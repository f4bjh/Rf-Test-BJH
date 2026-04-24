library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity status_reg_block is
  port (
    clk   : in std_logic;
    reset_n : in std_logic;

    fifo_count        : in std_logic_vector(7 downto 0);
    fifo_full         : in std_logic;
    error_flag        : in std_logic;
    nco_freq_valid    : in std_logic;

    led_on            : in std_logic;
    led_off           : in std_logic;
    led_toggle        : in std_logic;

    start_meas        : in std_logic;
    meas_done         : in std_logic;

    status_reg        : out std_logic_vector(31 downto 0)
  );
end entity;

architecture rtl of status_reg_block is

signal start_meas_status : std_logic := '0';
signal led_on_state : std_logic := '0';
signal led_off_state: std_logic :='0';

begin

-- build register
status_reg(31 downto 24) <= x"00";
status_reg(23 downto 16) <= x"03";
status_reg(15 downto 8)  <= fifo_count;

status_reg(7) <= start_meas_status;
status_reg(6) <= fifo_full;
status_reg(5) <= error_flag;
status_reg(4) <= nco_freq_valid;
status_reg(3) <= led_on_state;
status_reg(2) <= led_off_state;
status_reg(1) <= '0';
status_reg(0) <= '0';

-- start_meas status
process(clk)
begin
  if rising_edge(clk) then
    if reset_n = '0' then
      start_meas_status <= '0';
    else
      if start_meas = '1' then
        start_meas_status <= '1';
      elsif meas_done = '1' then
        start_meas_status <= '0';
      end if;
    end if;
  end if;
end process;

process(clk, reset_n)
begin
  if reset_n = '0' then
    led_on_state <= '0';
    led_off_state <= '0';
  elsif rising_edge(clk) then
    if led_on = '1' then
      led_on_state <= '1';
      led_off_state <='0';
    elsif led_off = '1' then
      led_on_state <= '0';
      led_off_state <='1';
    end if;
  end if;
end process;

end architecture;