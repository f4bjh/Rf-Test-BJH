library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

entity Stimu is  
Port ( 
   tb_clock12M : out std_logic;
   tb_reset : out std_logic
);
end Stimu;

architecture Behavioral of Stimu is
    constant  clock12M: time := 83 ns;   -- clock constant
    
begin

-- Clock
clock12M_process: process
begin
    tb_clock12M <= '0';
    wait for clock12M/2;
    tb_clock12M <= '1';
    wait for clock12M/2;
end process;

-- Reset
stim_proc: process
begin        
  tb_reset <= '0';
  wait for 100 ns;    
    tb_reset <= '1';
  wait for clock12M*10;
  wait;
end process;
end Behavioral;