-- Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
-- Copyright 2022-2024 Advanced Micro Devices, Inc. All Rights Reserved.
-- --------------------------------------------------------------------------------
-- Tool Version: Vivado v.2024.2 (lin64) Build 5239630 Fri Nov 08 22:34:34 MST 2024
-- Date        : Sat Sep 13 17:47:28 2025
-- Host        : f4bjh-minipc running 64-bit Ubuntu 24.04.3 LTS
-- Command     : write_vhdl -force -mode funcsim
--               /home/f4bjh/devel/Rf-Test-BJH/tests/hf_gen_fpga/Cmod_A7-hf_gen_fpga/Cmod_A7-hf_gen_fpga.gen/sources_1/bd/design_1/ip/design_1_oscillator2_0_0/design_1_oscillator2_0_0_sim_netlist.vhdl
-- Design      : design_1_oscillator2_0_0
-- Purpose     : This VHDL netlist is a functional simulation representation of the design and should not be modified or
--               synthesized. This netlist cannot be used for SDF annotated simulation.
-- Device      : xc7a35tcpg236-1
-- --------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
library UNISIM;
use UNISIM.VCOMPONENTS.ALL;
entity design_1_oscillator2_0_0_oscillator2 is
  port (
    clock12M : in STD_LOGIC;
    reset_n : in STD_LOGIC;
    clk : out STD_LOGIC;
    clkn : out STD_LOGIC
  );
  attribute MAX : string;
  attribute MAX of design_1_oscillator2_0_0_oscillator2 : entity is "8'b00000100";
  attribute ORIG_REF_NAME : string;
  attribute ORIG_REF_NAME of design_1_oscillator2_0_0_oscillator2 : entity is "oscillator2";
end design_1_oscillator2_0_0_oscillator2;

architecture STRUCTURE of design_1_oscillator2_0_0_oscillator2 is
  component design_1_oscillator2_0_0_clk_wiz_0 is
  port (
    clk_out1 : out STD_LOGIC;
    reset : in STD_LOGIC;
    locked : out STD_LOGIC;
    clk_in1 : in STD_LOGIC
  );
  end component design_1_oscillator2_0_0_clk_wiz_0;
  signal clk_sig : STD_LOGIC;
  signal clk_sig_d : STD_LOGIC;
  signal clk_sig_i_1_n_0 : STD_LOGIC;
  signal clkn_sig : STD_LOGIC;
  signal clkn_sig_i_1_n_0 : STD_LOGIC;
  signal counter : STD_LOGIC_VECTOR ( 15 downto 0 );
  signal \counter[0]_i_1_n_0\ : STD_LOGIC;
  signal \counter[0]_i_2_n_0\ : STD_LOGIC;
  signal \counter[10]_i_1_n_0\ : STD_LOGIC;
  signal \counter[11]_i_1_n_0\ : STD_LOGIC;
  signal \counter[12]_i_1_n_0\ : STD_LOGIC;
  signal \counter[13]_i_1_n_0\ : STD_LOGIC;
  signal \counter[14]_i_1_n_0\ : STD_LOGIC;
  signal \counter[15]_i_2_n_0\ : STD_LOGIC;
  signal \counter[15]_i_3_n_0\ : STD_LOGIC;
  signal \counter[15]_i_5_n_0\ : STD_LOGIC;
  signal \counter[15]_i_6_n_0\ : STD_LOGIC;
  signal \counter[15]_i_7_n_0\ : STD_LOGIC;
  signal \counter[15]_i_8_n_0\ : STD_LOGIC;
  signal \counter[1]_i_1_n_0\ : STD_LOGIC;
  signal \counter[2]_i_1_n_0\ : STD_LOGIC;
  signal \counter[3]_i_1_n_0\ : STD_LOGIC;
  signal \counter[4]_i_1_n_0\ : STD_LOGIC;
  signal \counter[5]_i_1_n_0\ : STD_LOGIC;
  signal \counter[6]_i_1_n_0\ : STD_LOGIC;
  signal \counter[7]_i_1_n_0\ : STD_LOGIC;
  signal \counter[8]_i_1_n_0\ : STD_LOGIC;
  signal \counter[9]_i_1_n_0\ : STD_LOGIC;
  signal counter_d : STD_LOGIC;
  signal \counter_reg[12]_i_2_n_0\ : STD_LOGIC;
  signal \counter_reg[12]_i_2_n_1\ : STD_LOGIC;
  signal \counter_reg[12]_i_2_n_2\ : STD_LOGIC;
  signal \counter_reg[12]_i_2_n_3\ : STD_LOGIC;
  signal \counter_reg[15]_i_4_n_2\ : STD_LOGIC;
  signal \counter_reg[15]_i_4_n_3\ : STD_LOGIC;
  signal \counter_reg[4]_i_2_n_0\ : STD_LOGIC;
  signal \counter_reg[4]_i_2_n_1\ : STD_LOGIC;
  signal \counter_reg[4]_i_2_n_2\ : STD_LOGIC;
  signal \counter_reg[4]_i_2_n_3\ : STD_LOGIC;
  signal \counter_reg[8]_i_2_n_0\ : STD_LOGIC;
  signal \counter_reg[8]_i_2_n_1\ : STD_LOGIC;
  signal \counter_reg[8]_i_2_n_2\ : STD_LOGIC;
  signal \counter_reg[8]_i_2_n_3\ : STD_LOGIC;
  signal data0 : STD_LOGIC_VECTOR ( 15 downto 1 );
  signal interlock : STD_LOGIC;
  signal reset : STD_LOGIC;
  signal \NLW_counter_reg[15]_i_4_CO_UNCONNECTED\ : STD_LOGIC_VECTOR ( 3 downto 2 );
  signal \NLW_counter_reg[15]_i_4_O_UNCONNECTED\ : STD_LOGIC_VECTOR ( 3 to 3 );
  signal NLW_u_clk_clk_out1_UNCONNECTED : STD_LOGIC;
  signal NLW_u_clk_locked_UNCONNECTED : STD_LOGIC;
  attribute SOFT_HLUTNM : string;
  attribute SOFT_HLUTNM of clk_INST_0 : label is "soft_lutpair0";
  attribute SOFT_HLUTNM of clk_sig_i_1 : label is "soft_lutpair8";
  attribute SOFT_HLUTNM of clkn_sig_i_1 : label is "soft_lutpair0";
  attribute SOFT_HLUTNM of \counter[10]_i_1\ : label is "soft_lutpair2";
  attribute SOFT_HLUTNM of \counter[11]_i_1\ : label is "soft_lutpair3";
  attribute SOFT_HLUTNM of \counter[12]_i_1\ : label is "soft_lutpair3";
  attribute SOFT_HLUTNM of \counter[13]_i_1\ : label is "soft_lutpair2";
  attribute SOFT_HLUTNM of \counter[14]_i_1\ : label is "soft_lutpair1";
  attribute SOFT_HLUTNM of \counter[15]_i_2\ : label is "soft_lutpair1";
  attribute SOFT_HLUTNM of \counter[2]_i_1\ : label is "soft_lutpair7";
  attribute SOFT_HLUTNM of \counter[3]_i_1\ : label is "soft_lutpair7";
  attribute SOFT_HLUTNM of \counter[4]_i_1\ : label is "soft_lutpair6";
  attribute SOFT_HLUTNM of \counter[5]_i_1\ : label is "soft_lutpair6";
  attribute SOFT_HLUTNM of \counter[6]_i_1\ : label is "soft_lutpair4";
  attribute SOFT_HLUTNM of \counter[7]_i_1\ : label is "soft_lutpair5";
  attribute SOFT_HLUTNM of \counter[8]_i_1\ : label is "soft_lutpair5";
  attribute SOFT_HLUTNM of \counter[9]_i_1\ : label is "soft_lutpair4";
  attribute ADDER_THRESHOLD : integer;
  attribute ADDER_THRESHOLD of \counter_reg[12]_i_2\ : label is 35;
  attribute ADDER_THRESHOLD of \counter_reg[15]_i_4\ : label is 35;
  attribute ADDER_THRESHOLD of \counter_reg[4]_i_2\ : label is 35;
  attribute ADDER_THRESHOLD of \counter_reg[8]_i_2\ : label is 35;
  attribute SOFT_HLUTNM of interlock_i_1 : label is "soft_lutpair8";
begin
clk_INST_0: unisim.vcomponents.LUT2
    generic map(
      INIT => X"2"
    )
        port map (
      I0 => clk_sig,
      I1 => interlock,
      O => clk
    );
clk_sig_i_1: unisim.vcomponents.LUT2
    generic map(
      INIT => X"9"
    )
        port map (
      I0 => \counter[15]_i_3_n_0\,
      I1 => clk_sig,
      O => clk_sig_i_1_n_0
    );
clk_sig_reg: unisim.vcomponents.FDCE
     port map (
      C => clock12M,
      CE => '1',
      CLR => reset,
      D => clk_sig_i_1_n_0,
      Q => clk_sig
    );
clkn_INST_0: unisim.vcomponents.LUT2
    generic map(
      INIT => X"2"
    )
        port map (
      I0 => clkn_sig,
      I1 => interlock,
      O => clkn
    );
clkn_sig_i_1: unisim.vcomponents.LUT3
    generic map(
      INIT => X"E2"
    )
        port map (
      I0 => clk_sig,
      I1 => \counter[15]_i_3_n_0\,
      I2 => clkn_sig,
      O => clkn_sig_i_1_n_0
    );
clkn_sig_reg: unisim.vcomponents.FDPE
     port map (
      C => clock12M,
      CE => '1',
      D => clkn_sig_i_1_n_0,
      PRE => reset,
      Q => clkn_sig
    );
\counter[0]_i_1\: unisim.vcomponents.LUT2
    generic map(
      INIT => X"2"
    )
        port map (
      I0 => \counter[0]_i_2_n_0\,
      I1 => counter(0),
      O => \counter[0]_i_1_n_0\
    );
\counter[0]_i_2\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"FFFFFFFFFFFFFFFE"
    )
        port map (
      I0 => \counter[15]_i_8_n_0\,
      I1 => \counter[15]_i_7_n_0\,
      I2 => counter(15),
      I3 => counter(14),
      I4 => counter(1),
      I5 => \counter[15]_i_5_n_0\,
      O => \counter[0]_i_2_n_0\
    );
\counter[10]_i_1\: unisim.vcomponents.LUT2
    generic map(
      INIT => X"8"
    )
        port map (
      I0 => \counter[15]_i_3_n_0\,
      I1 => data0(10),
      O => \counter[10]_i_1_n_0\
    );
\counter[11]_i_1\: unisim.vcomponents.LUT2
    generic map(
      INIT => X"8"
    )
        port map (
      I0 => \counter[15]_i_3_n_0\,
      I1 => data0(11),
      O => \counter[11]_i_1_n_0\
    );
\counter[12]_i_1\: unisim.vcomponents.LUT2
    generic map(
      INIT => X"8"
    )
        port map (
      I0 => \counter[15]_i_3_n_0\,
      I1 => data0(12),
      O => \counter[12]_i_1_n_0\
    );
\counter[13]_i_1\: unisim.vcomponents.LUT2
    generic map(
      INIT => X"8"
    )
        port map (
      I0 => \counter[15]_i_3_n_0\,
      I1 => data0(13),
      O => \counter[13]_i_1_n_0\
    );
\counter[14]_i_1\: unisim.vcomponents.LUT2
    generic map(
      INIT => X"8"
    )
        port map (
      I0 => \counter[15]_i_3_n_0\,
      I1 => data0(14),
      O => \counter[14]_i_1_n_0\
    );
\counter[15]_i_1\: unisim.vcomponents.LUT2
    generic map(
      INIT => X"7"
    )
        port map (
      I0 => \counter[15]_i_3_n_0\,
      I1 => interlock,
      O => counter_d
    );
\counter[15]_i_2\: unisim.vcomponents.LUT2
    generic map(
      INIT => X"8"
    )
        port map (
      I0 => \counter[15]_i_3_n_0\,
      I1 => data0(15),
      O => \counter[15]_i_2_n_0\
    );
\counter[15]_i_3\: unisim.vcomponents.LUT5
    generic map(
      INIT => X"FFFFFFFE"
    )
        port map (
      I0 => \counter[15]_i_5_n_0\,
      I1 => \counter[15]_i_6_n_0\,
      I2 => \counter[15]_i_7_n_0\,
      I3 => \counter[15]_i_8_n_0\,
      I4 => counter(0),
      O => \counter[15]_i_3_n_0\
    );
\counter[15]_i_5\: unisim.vcomponents.LUT4
    generic map(
      INIT => X"FFFE"
    )
        port map (
      I0 => counter(11),
      I1 => counter(10),
      I2 => counter(13),
      I3 => counter(12),
      O => \counter[15]_i_5_n_0\
    );
\counter[15]_i_6\: unisim.vcomponents.LUT3
    generic map(
      INIT => X"FE"
    )
        port map (
      I0 => counter(15),
      I1 => counter(14),
      I2 => counter(1),
      O => \counter[15]_i_6_n_0\
    );
\counter[15]_i_7\: unisim.vcomponents.LUT4
    generic map(
      INIT => X"FFFD"
    )
        port map (
      I0 => counter(2),
      I1 => counter(3),
      I2 => counter(5),
      I3 => counter(4),
      O => \counter[15]_i_7_n_0\
    );
\counter[15]_i_8\: unisim.vcomponents.LUT4
    generic map(
      INIT => X"FFFE"
    )
        port map (
      I0 => counter(7),
      I1 => counter(6),
      I2 => counter(9),
      I3 => counter(8),
      O => \counter[15]_i_8_n_0\
    );
\counter[1]_i_1\: unisim.vcomponents.LUT2
    generic map(
      INIT => X"8"
    )
        port map (
      I0 => \counter[15]_i_3_n_0\,
      I1 => data0(1),
      O => \counter[1]_i_1_n_0\
    );
\counter[2]_i_1\: unisim.vcomponents.LUT2
    generic map(
      INIT => X"8"
    )
        port map (
      I0 => \counter[15]_i_3_n_0\,
      I1 => data0(2),
      O => \counter[2]_i_1_n_0\
    );
\counter[3]_i_1\: unisim.vcomponents.LUT2
    generic map(
      INIT => X"8"
    )
        port map (
      I0 => \counter[15]_i_3_n_0\,
      I1 => data0(3),
      O => \counter[3]_i_1_n_0\
    );
\counter[4]_i_1\: unisim.vcomponents.LUT2
    generic map(
      INIT => X"8"
    )
        port map (
      I0 => \counter[15]_i_3_n_0\,
      I1 => data0(4),
      O => \counter[4]_i_1_n_0\
    );
\counter[5]_i_1\: unisim.vcomponents.LUT2
    generic map(
      INIT => X"8"
    )
        port map (
      I0 => \counter[15]_i_3_n_0\,
      I1 => data0(5),
      O => \counter[5]_i_1_n_0\
    );
\counter[6]_i_1\: unisim.vcomponents.LUT2
    generic map(
      INIT => X"8"
    )
        port map (
      I0 => \counter[15]_i_3_n_0\,
      I1 => data0(6),
      O => \counter[6]_i_1_n_0\
    );
\counter[7]_i_1\: unisim.vcomponents.LUT2
    generic map(
      INIT => X"8"
    )
        port map (
      I0 => \counter[15]_i_3_n_0\,
      I1 => data0(7),
      O => \counter[7]_i_1_n_0\
    );
\counter[8]_i_1\: unisim.vcomponents.LUT2
    generic map(
      INIT => X"8"
    )
        port map (
      I0 => \counter[15]_i_3_n_0\,
      I1 => data0(8),
      O => \counter[8]_i_1_n_0\
    );
\counter[9]_i_1\: unisim.vcomponents.LUT2
    generic map(
      INIT => X"8"
    )
        port map (
      I0 => \counter[15]_i_3_n_0\,
      I1 => data0(9),
      O => \counter[9]_i_1_n_0\
    );
\counter_reg[0]\: unisim.vcomponents.FDCE
     port map (
      C => clock12M,
      CE => counter_d,
      CLR => reset,
      D => \counter[0]_i_1_n_0\,
      Q => counter(0)
    );
\counter_reg[10]\: unisim.vcomponents.FDCE
     port map (
      C => clock12M,
      CE => counter_d,
      CLR => reset,
      D => \counter[10]_i_1_n_0\,
      Q => counter(10)
    );
\counter_reg[11]\: unisim.vcomponents.FDCE
     port map (
      C => clock12M,
      CE => counter_d,
      CLR => reset,
      D => \counter[11]_i_1_n_0\,
      Q => counter(11)
    );
\counter_reg[12]\: unisim.vcomponents.FDCE
     port map (
      C => clock12M,
      CE => counter_d,
      CLR => reset,
      D => \counter[12]_i_1_n_0\,
      Q => counter(12)
    );
\counter_reg[12]_i_2\: unisim.vcomponents.CARRY4
     port map (
      CI => \counter_reg[8]_i_2_n_0\,
      CO(3) => \counter_reg[12]_i_2_n_0\,
      CO(2) => \counter_reg[12]_i_2_n_1\,
      CO(1) => \counter_reg[12]_i_2_n_2\,
      CO(0) => \counter_reg[12]_i_2_n_3\,
      CYINIT => '0',
      DI(3 downto 0) => B"0000",
      O(3 downto 0) => data0(12 downto 9),
      S(3 downto 0) => counter(12 downto 9)
    );
\counter_reg[13]\: unisim.vcomponents.FDCE
     port map (
      C => clock12M,
      CE => counter_d,
      CLR => reset,
      D => \counter[13]_i_1_n_0\,
      Q => counter(13)
    );
\counter_reg[14]\: unisim.vcomponents.FDCE
     port map (
      C => clock12M,
      CE => counter_d,
      CLR => reset,
      D => \counter[14]_i_1_n_0\,
      Q => counter(14)
    );
\counter_reg[15]\: unisim.vcomponents.FDCE
     port map (
      C => clock12M,
      CE => counter_d,
      CLR => reset,
      D => \counter[15]_i_2_n_0\,
      Q => counter(15)
    );
\counter_reg[15]_i_4\: unisim.vcomponents.CARRY4
     port map (
      CI => \counter_reg[12]_i_2_n_0\,
      CO(3 downto 2) => \NLW_counter_reg[15]_i_4_CO_UNCONNECTED\(3 downto 2),
      CO(1) => \counter_reg[15]_i_4_n_2\,
      CO(0) => \counter_reg[15]_i_4_n_3\,
      CYINIT => '0',
      DI(3 downto 0) => B"0000",
      O(3) => \NLW_counter_reg[15]_i_4_O_UNCONNECTED\(3),
      O(2 downto 0) => data0(15 downto 13),
      S(3) => '0',
      S(2 downto 0) => counter(15 downto 13)
    );
\counter_reg[1]\: unisim.vcomponents.FDCE
     port map (
      C => clock12M,
      CE => counter_d,
      CLR => reset,
      D => \counter[1]_i_1_n_0\,
      Q => counter(1)
    );
\counter_reg[2]\: unisim.vcomponents.FDCE
     port map (
      C => clock12M,
      CE => counter_d,
      CLR => reset,
      D => \counter[2]_i_1_n_0\,
      Q => counter(2)
    );
\counter_reg[3]\: unisim.vcomponents.FDCE
     port map (
      C => clock12M,
      CE => counter_d,
      CLR => reset,
      D => \counter[3]_i_1_n_0\,
      Q => counter(3)
    );
\counter_reg[4]\: unisim.vcomponents.FDCE
     port map (
      C => clock12M,
      CE => counter_d,
      CLR => reset,
      D => \counter[4]_i_1_n_0\,
      Q => counter(4)
    );
\counter_reg[4]_i_2\: unisim.vcomponents.CARRY4
     port map (
      CI => '0',
      CO(3) => \counter_reg[4]_i_2_n_0\,
      CO(2) => \counter_reg[4]_i_2_n_1\,
      CO(1) => \counter_reg[4]_i_2_n_2\,
      CO(0) => \counter_reg[4]_i_2_n_3\,
      CYINIT => counter(0),
      DI(3 downto 0) => B"0000",
      O(3 downto 0) => data0(4 downto 1),
      S(3 downto 0) => counter(4 downto 1)
    );
\counter_reg[5]\: unisim.vcomponents.FDCE
     port map (
      C => clock12M,
      CE => counter_d,
      CLR => reset,
      D => \counter[5]_i_1_n_0\,
      Q => counter(5)
    );
\counter_reg[6]\: unisim.vcomponents.FDCE
     port map (
      C => clock12M,
      CE => counter_d,
      CLR => reset,
      D => \counter[6]_i_1_n_0\,
      Q => counter(6)
    );
\counter_reg[7]\: unisim.vcomponents.FDCE
     port map (
      C => clock12M,
      CE => counter_d,
      CLR => reset,
      D => \counter[7]_i_1_n_0\,
      Q => counter(7)
    );
\counter_reg[8]\: unisim.vcomponents.FDCE
     port map (
      C => clock12M,
      CE => counter_d,
      CLR => reset,
      D => \counter[8]_i_1_n_0\,
      Q => counter(8)
    );
\counter_reg[8]_i_2\: unisim.vcomponents.CARRY4
     port map (
      CI => \counter_reg[4]_i_2_n_0\,
      CO(3) => \counter_reg[8]_i_2_n_0\,
      CO(2) => \counter_reg[8]_i_2_n_1\,
      CO(1) => \counter_reg[8]_i_2_n_2\,
      CO(0) => \counter_reg[8]_i_2_n_3\,
      CYINIT => '0',
      DI(3 downto 0) => B"0000",
      O(3 downto 0) => data0(8 downto 5),
      S(3 downto 0) => counter(8 downto 5)
    );
\counter_reg[9]\: unisim.vcomponents.FDCE
     port map (
      C => clock12M,
      CE => counter_d,
      CLR => reset,
      D => \counter[9]_i_1_n_0\,
      Q => counter(9)
    );
interlock_i_1: unisim.vcomponents.LUT1
    generic map(
      INIT => X"1"
    )
        port map (
      I0 => \counter[15]_i_3_n_0\,
      O => clk_sig_d
    );
interlock_reg: unisim.vcomponents.FDCE
     port map (
      C => clock12M,
      CE => '1',
      CLR => reset,
      D => clk_sig_d,
      Q => interlock
    );
u_clk: component design_1_oscillator2_0_0_clk_wiz_0
     port map (
      clk_in1 => clock12M,
      clk_out1 => NLW_u_clk_clk_out1_UNCONNECTED,
      locked => NLW_u_clk_locked_UNCONNECTED,
      reset => reset
    );
u_clk_i_1: unisim.vcomponents.LUT1
    generic map(
      INIT => X"1"
    )
        port map (
      I0 => reset_n,
      O => reset
    );
end STRUCTURE;
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
library UNISIM;
use UNISIM.VCOMPONENTS.ALL;
entity design_1_oscillator2_0_0 is
  port (
    clock12M : in STD_LOGIC;
    reset_n : in STD_LOGIC;
    clk : out STD_LOGIC;
    clkn : out STD_LOGIC
  );
  attribute NotValidForBitStream : boolean;
  attribute NotValidForBitStream of design_1_oscillator2_0_0 : entity is true;
  attribute CHECK_LICENSE_TYPE : string;
  attribute CHECK_LICENSE_TYPE of design_1_oscillator2_0_0 : entity is "design_1_oscillator2_0_0,oscillator2,{}";
  attribute DowngradeIPIdentifiedWarnings : string;
  attribute DowngradeIPIdentifiedWarnings of design_1_oscillator2_0_0 : entity is "yes";
  attribute IP_DEFINITION_SOURCE : string;
  attribute IP_DEFINITION_SOURCE of design_1_oscillator2_0_0 : entity is "module_ref";
  attribute X_CORE_INFO : string;
  attribute X_CORE_INFO of design_1_oscillator2_0_0 : entity is "oscillator2,Vivado 2024.2";
end design_1_oscillator2_0_0;

architecture STRUCTURE of design_1_oscillator2_0_0 is
  attribute MAX : string;
  attribute MAX of inst : label is "8'b00000100";
  attribute X_INTERFACE_INFO : string;
  attribute X_INTERFACE_INFO of clk : signal is "xilinx.com:signal:clock:1.0 clk CLK";
  attribute X_INTERFACE_MODE : string;
  attribute X_INTERFACE_MODE of clk : signal is "master";
  attribute X_INTERFACE_PARAMETER : string;
  attribute X_INTERFACE_PARAMETER of clk : signal is "XIL_INTERFACENAME clk, FREQ_HZ 100000000, FREQ_TOLERANCE_HZ 0, PHASE 0.0, CLK_DOMAIN design_1_oscillator2_0_0_clk, INSERT_VIP 0";
  attribute X_INTERFACE_INFO of reset_n : signal is "xilinx.com:signal:reset:1.0 reset_n RST";
  attribute X_INTERFACE_MODE of reset_n : signal is "slave";
  attribute X_INTERFACE_PARAMETER of reset_n : signal is "XIL_INTERFACENAME reset_n, POLARITY ACTIVE_LOW, INSERT_VIP 0";
begin
inst: entity work.design_1_oscillator2_0_0_oscillator2
     port map (
      clk => clk,
      clkn => clkn,
      clock12M => clock12M,
      reset_n => reset_n
    );
end STRUCTURE;
