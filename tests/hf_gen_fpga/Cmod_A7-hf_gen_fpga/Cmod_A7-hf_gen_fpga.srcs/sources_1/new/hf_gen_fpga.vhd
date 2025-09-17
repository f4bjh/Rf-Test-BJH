---------------------------------
-- Company: MisCircuitos.com
-- Engineer: Alberto Lopez
-- alberto@miscircuitos.com
-- WWW.MISCIRCUITOS.COM

-- Create Date:    12:09:49 06/18/2020 
-- Module Name:    oscillator- Behavioral 
-- Description: Frequency divider with interlock delay time
-- OUTPUTS
--  --> 2 signals with clk and clkn (non-overlapping)
----------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.numeric_std.all;

entity oscillator2 is
generic(
    MAX: unsigned (7 downto 0) := x"01"
);

port(
    clock12M: in std_logic;    --internal clock input
    reset_n:     in std_logic;
    clk:    out std_logic;
    clkn:    out std_logic
);
end oscillator2;

architecture Behavioral of oscillator2 is

  -- Déclaration du composant Clocking Wizard
   component clk_wiz_0
     port (
      clk_in1  : in  std_logic;
      clk_out1 : out std_logic;
      reset    : in  std_logic;
      locked   : out std_logic
     );
   end component;

--signals
    signal clock50M   : std_logic;
    signal counter: unsigned (15 downto 0);
    signal counter_d: unsigned (15 downto 0);    
    
    signal clk_master: std_logic;
    signal clk_sig: std_logic;
    signal clkn_sig: std_logic;
    signal clk_sig_d: std_logic;
    signal clkn_sig_d: std_logic;
    signal reset: std_logic;
    signal locked  : std_logic;
    signal interlock: std_logic;
    signal interlock_d: std_logic;
begin

 -- Instanciation du Clocking Wizard
 u_clk : clk_wiz_0
    port map (
      clk_in1  => clock12M,
      clk_out1 => clock50M,
      reset    => reset,
      locked   => locked
    );

p_combinatorial : process (counter, clk_sig, clkn_sig, interlock) begin    
if(counter = MAX) then
        counter_d <= (others => '0');
        interlock_d <= '1';
        if(clk_sig ='0') then 
            clk_sig_d <= '1';
            clkn_sig_d <= '0';
        else
            clk_sig_d <= '0';
            clkn_sig_d <= '1';    
        end if;

    elsif (interlock = '0') then
        counter_d <= counter + 1;
        clk_sig_d <= clk_sig;
        clkn_sig_d <= clkn_sig;
        interlock_d <= '0';
    else
        counter_d <= counter;
        clk_sig_d <= clk_sig;
        clkn_sig_d <= clkn_sig;
        interlock_d <= '0';
    end if;        
end process;


p_clock: process( clk_master, reset_n ) begin
    if(reset_n = '0') then    --RESET
        counter <= (others => '0');
        clk_sig    <= '0';
        clkn_sig <= '1';
        interlock <= '0';
    
    elsif(clk_master'event and clk_master= '1') then
        counter <= counter_d;
        clk_sig <= clk_sig_d;
        clkn_sig <= clkn_sig_d;
        interlock <= interlock_d;
    end if;
end process;

clk_master <= clock12M;
reset <= not reset_n;
clk <= clk_sig and not(interlock);
clkn <= clkn_sig and not(interlock);

end Behavioral;