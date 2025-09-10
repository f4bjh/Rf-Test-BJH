----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 09/10/2025 09:33:13 PM
-- Design Name: 
-- Module Name: hf_gen_fpga - Behavioral
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

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;


use IEEE.numeric_std.all;
entity hf_gen_fpga is
generic(
    MAX: unsigned (7 downto 0) := "01000000"
);
port(
    clk_pin: in std_logic;    -- horloge externe (12 MHz)
    reset:     in std_logic;
    clk:    out std_logic;
    clkn:    out std_logic
);
end hf_gen_fpga;

architecture Behavioral of hf_gen_fpga is

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
    signal clock50M: std_logic;
    signal reset_n : std_logic := '0';
    signal locked  : std_logic;
    
    signal counter: unsigned (15 downto 0);
    signal counter_d: unsigned (15 downto 0);    
    
    signal clk_sig: std_logic;
    signal clkn_sig: std_logic;
    signal clk_sig_d: std_logic;
    signal clkn_sig_d: std_logic;
    signal interlock: std_logic;
    signal interlock_d: std_logic;
    
begin

  -- Instanciation du Clocking Wizard
  u_clk : clk_wiz_0
    port map (
      clk_in1  => clk_pin,
      clk_out1 => clock50M,
      reset    => reset_n,
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
  
  p_clock: process( clock50M, reset ) begin
    if(reset = '0') then    --RESET
        counter <= (others => '0');
        clk_sig    <= '0';
        clkn_sig <= '1';
        interlock <= '0';
    
    elsif(clock50M'event and clock50M= '1') then
        counter <= counter_d;
        clk_sig <= clk_sig_d;
        clkn_sig <= clkn_sig_d;
        interlock <= interlock_d;
    end if;
  end process;
  
clk <= clk_sig and not(interlock);
clkn <= clkn_sig and not(interlock);
end Behavioral;