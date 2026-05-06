-- Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
-- Copyright 2022-2024 Advanced Micro Devices, Inc. All Rights Reserved.
-- --------------------------------------------------------------------------------
-- Tool Version: Vivado v.2024.2 (lin64) Build 5239630 Fri Nov 08 22:34:34 MST 2024
-- Date        : Tue Sep 16 23:16:55 2025
-- Host        : f4bjh-minipc running 64-bit Ubuntu 24.04.3 LTS
-- Command     : write_vhdl -force -mode synth_stub
--               /home/f4bjh/devel/Rf-Test-BJH/tests/hf_gen_fpga/Cmod_A7-hf_gen_fpga/Cmod_A7-hf_gen_fpga.gen/sources_1/bd/design_1/ip/design_1_Stimu2_0_0/design_1_Stimu2_0_0_stub.vhdl
-- Design      : design_1_Stimu2_0_0
-- Purpose     : Stub declaration of top-level module interface
-- Device      : xc7a35tcpg236-1
-- --------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

entity design_1_Stimu2_0_0 is
  Port ( 
    tb_clock12M : out STD_LOGIC;
    tb_reset : out STD_LOGIC
  );

  attribute CHECK_LICENSE_TYPE : string;
  attribute CHECK_LICENSE_TYPE of design_1_Stimu2_0_0 : entity is "design_1_Stimu2_0_0,Stimu2,{}";
  attribute CORE_GENERATION_INFO : string;
  attribute CORE_GENERATION_INFO of design_1_Stimu2_0_0 : entity is "design_1_Stimu2_0_0,Stimu2,{x_ipProduct=Vivado 2024.2,x_ipVendor=xilinx.com,x_ipLibrary=module_ref,x_ipName=Stimu2,x_ipVersion=1.0,x_ipCoreRevision=1,x_ipLanguage=VERILOG,x_ipSimLanguage=MIXED}";
  attribute DowngradeIPIdentifiedWarnings : string;
  attribute DowngradeIPIdentifiedWarnings of design_1_Stimu2_0_0 : entity is "yes";
  attribute IP_DEFINITION_SOURCE : string;
  attribute IP_DEFINITION_SOURCE of design_1_Stimu2_0_0 : entity is "module_ref";
end design_1_Stimu2_0_0;

architecture stub of design_1_Stimu2_0_0 is
  attribute syn_black_box : boolean;
  attribute black_box_pad_pin : string;
  attribute syn_black_box of stub : architecture is true;
  attribute black_box_pad_pin of stub : architecture is "tb_clock12M,tb_reset";
  attribute X_INTERFACE_INFO : string;
  attribute X_INTERFACE_INFO of tb_reset : signal is "xilinx.com:signal:reset:1.0 tb_reset RST";
  attribute X_INTERFACE_MODE : string;
  attribute X_INTERFACE_MODE of tb_reset : signal is "master";
  attribute X_INTERFACE_PARAMETER : string;
  attribute X_INTERFACE_PARAMETER of tb_reset : signal is "XIL_INTERFACENAME tb_reset, POLARITY ACTIVE_LOW, INSERT_VIP 0";
  attribute X_CORE_INFO : string;
  attribute X_CORE_INFO of stub : architecture is "Stimu2,Vivado 2024.2";
begin
end;
