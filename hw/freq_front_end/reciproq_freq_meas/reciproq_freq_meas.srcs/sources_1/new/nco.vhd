library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity nco is
   generic(
      clk_freq : real := 100_000_000.0;  -- fréquence d'entrée (Hz)
      freq     : real := 100_000.0       -- fréquence de sortie (Hz)
   );
   port(
      clk_in  : in  std_logic;
      reset_n : in  std_logic;
      clk_out : out std_logic
   );
end entity;

architecture rtl of nco is
   constant PHASE_BITS : integer := 32;
   constant PHASE_INC  : unsigned(PHASE_BITS-1 downto 0) :=
      to_unsigned(integer((2.0**PHASE_BITS) * freq / clk_freq), PHASE_BITS);

   signal phase_acc : unsigned(PHASE_BITS-1 downto 0) := (others => '0');
   signal out_reg   : std_logic := '0';
begin
   process(clk_in, reset_n)
   begin
      if reset_n = '0' then
         phase_acc <= (others => '0');
         out_reg   <= '0';
      elsif rising_edge(clk_in) then
         phase_acc <= phase_acc + PHASE_INC;
         out_reg <= phase_acc(PHASE_BITS-1); -- MSB = signal carré
      end if;
   end process;

   clk_out <= out_reg;
end architecture;
