//Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
//Copyright 2022-2024 Advanced Micro Devices, Inc. All Rights Reserved.
//--------------------------------------------------------------------------------
//Tool Version: Vivado v.2024.2 (lin64) Build 5239630 Fri Nov 08 22:34:34 MST 2024
//Date        : Wed Sep 10 22:55:34 2025
//Host        : f4bjh-minipc running 64-bit Ubuntu 24.04.3 LTS
//Command     : generate_target tb_design_1.bd
//Design      : tb_design_1
//Purpose     : IP block netlist
//--------------------------------------------------------------------------------
`timescale 1 ps / 1 ps

(* CORE_GENERATION_INFO = "tb_design_1,IP_Integrator,{x_ipVendor=xilinx.com,x_ipLibrary=BlockDiagram,x_ipName=tb_design_1,x_ipVersion=1.00.a,x_ipLanguage=VERILOG,numBlks=2,numReposBlks=2,numNonXlnxBlks=0,numHierBlks=0,maxHierDepth=0,numSysgenBlks=0,numHlsBlks=0,numHdlrefBlks=2,numPkgbdBlks=0,bdsource=USER,synth_mode=Hierarchical}" *) (* HW_HANDOFF = "tb_design_1.hwdef" *) 
module tb_design_1
   ();

  wire Stimu_0_tb_clock12M;
  wire Stimu_0_tb_reset;

  tb_design_1_Stimu_0_0 Stimu_0
       (.tb_clock12M(Stimu_0_tb_clock12M),
        .tb_reset(Stimu_0_tb_reset));
  tb_design_1_hf_gen_fpga_0_0 hf_gen_fpga_0
       (.clk_pin(Stimu_0_tb_clock12M),
        .reset(Stimu_0_tb_reset));
endmodule
