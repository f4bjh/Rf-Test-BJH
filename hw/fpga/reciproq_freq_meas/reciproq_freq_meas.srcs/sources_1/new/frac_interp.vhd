-- frac_interp.vhd
-- Fractional edge interpolation filter to improve frequency resolution
library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity frac_interp is
  generic (
    INTERP_STAGES : natural := 8
  );
  port (
    clk        : in  std_logic;
    rst_n      : in  std_logic;
    valid_in   : in  std_logic;
    raw_period : in  unsigned(63 downto 0);
    interp_out : out unsigned(63 downto 0);
    valid_out  : out std_logic
  );
end entity;

architecture rtl of frac_interp is
  type pipe_t is array (0 to INTERP_STAGES-1) of unsigned(63 downto 0);
  signal pipe        : pipe_t := (others => (others => '0'));
  signal valid_pipe  : std_logic_vector(INTERP_STAGES-1 downto 0) := (others => '0');
  signal sum         : unsigned(63 downto 0) := (others => '0');
  signal interp_out_r       : unsigned(63 downto 0);
  signal valid_out_r         :std_logic;
  
begin
  process(clk)
    variable sum_v : unsigned(63 downto 0);
  begin
    if rising_edge(clk) then
      if rst_n = '0' then
        pipe        <= (others => (others => '0'));
        valid_pipe  <= (others => '0');
        sum         <= (others => '0');
        interp_out_r  <= (others => '0');
        valid_out_r   <= '0';
      else
        if valid_in = '1' then
          pipe(0) <= raw_period;
        end if;
        for i in 1 to INTERP_STAGES-1 loop
          pipe(i) <= pipe(i-1);
        end loop;
        valid_pipe <= valid_pipe(INTERP_STAGES-2 downto 0) & valid_in;

        -- somme pipeline
        sum_v := (others => '0');
        for i in 0 to INTERP_STAGES-1 loop
          sum_v := sum_v + pipe(i);
        end loop;

        -- latch sortie et valid ensemble
        sum        <= sum_v;
        interp_out_r <= sum_v / to_unsigned(INTERP_STAGES, 64);
        valid_out_r  <= valid_pipe(INTERP_STAGES-1);
      end if;
    end if;
  end process;
  
  interp_out <= interp_out_r;
  valid_out <= valid_out_r;
  
end architecture;
