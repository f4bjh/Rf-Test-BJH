----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 09/09/2025 10:28:41 PM
-- Design Name: 
-- Module Name: top_blinky - Behavioral
-- Project Name: 
-- Target Devices: 
-- Tool Versions: 
-- Description: 
-- 
-- Dependencies: 
-- 
-- Revision:
-- Revision 0.01 - File Created
-- Additional Comments:
-- 
----------------------------------------------------------------------------------


library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity top_blinky is
  Port (
    clk_pin : in  std_logic;   -- horloge externe (12 MHz)
    led_o   : out std_logic    -- sortie vers LED
  );
end top_blinky;

architecture RTL of top_blinky is
  -- Déclaration du composant Clocking Wizard
  component clk_wiz_0
    port (
      clk_in1  : in  std_logic;
      clk_out1 : out std_logic;
      reset    : in  std_logic;
      locked   : out std_logic
    );
  end component;

  signal clk25   : std_logic;
  signal reset_n : std_logic := '0';
  signal locked  : std_logic;

  signal counter : unsigned(24 downto 0) := (others => '0');
  signal led_reg : std_logic := '0';

begin
    -- Instanciation du Clocking Wizard
    u_clk : clk_wiz_0
      port map (
        clk_in1  => clk_pin,
        clk_out1 => clk25,
        reset    => reset_n,
        locked   => locked
      );
  
    -- Process de clignotement
    process(clk25)
    begin
      if rising_edge(clk25) then
        counter <= counter + 1;
        if counter = 12500000 then  -- 25 MHz / 12.5M = 2 Hz → toggle toutes 0.5 s
          led_reg <= not led_reg;
          counter <= (others => '0');
        end if;
      end if;
    end process;
  
    led_o <= led_reg;
  
end RTL;