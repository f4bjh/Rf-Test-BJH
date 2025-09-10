-- Testbench pour top_blinky
-- Ne sera jamais synthétisé, uniquement simulé avec XSim

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

entity tb_top_blinky is
-- pas de ports dans un testbench
end tb_top_blinky;

architecture sim of tb_top_blinky is

  -- Signaux internes reliés au DUT (Device Under Test)
  signal clk_tb  : std_logic := '0';
  signal led_tb  : std_logic;

  -- Période de l'horloge (12 MHz -> 83,333 ns)
  constant CLK_PERIOD : time := 83 ns;

begin

  -------------------------------------------------------------------
  -- Génération d'horloge
  -------------------------------------------------------------------
  clk_process : process
  begin
    clk_tb <= '0';
    wait for CLK_PERIOD/2;
    clk_tb <= '1';
    wait for CLK_PERIOD/2;
  end process;

  -------------------------------------------------------------------
  -- Instanciation du DUT
  -------------------------------------------------------------------
  uut: entity work.top_blinky
    port map (
      clk_pin => clk_tb,   -- entrée d'horloge
      led_o   => led_tb    -- sortie LED
    );

  -------------------------------------------------------------------
  -- Stimuli (optionnel)
  -------------------------------------------------------------------
  stim_proc : process
  begin
    -- Laisse tourner la simulation pendant 10 ms
    wait for 10 ms;

    -- Stop simulation
    assert false report "Fin de la simulation" severity failure;
  end process;

end sim;