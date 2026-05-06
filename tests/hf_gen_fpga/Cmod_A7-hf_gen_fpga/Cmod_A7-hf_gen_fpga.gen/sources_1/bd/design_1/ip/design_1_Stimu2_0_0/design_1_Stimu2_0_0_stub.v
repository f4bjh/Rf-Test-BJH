// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// Copyright 2022-2024 Advanced Micro Devices, Inc. All Rights Reserved.
// --------------------------------------------------------------------------------
// Tool Version: Vivado v.2024.2 (lin64) Build 5239630 Fri Nov 08 22:34:34 MST 2024
// Date        : Tue Sep 16 23:16:55 2025
// Host        : f4bjh-minipc running 64-bit Ubuntu 24.04.3 LTS
// Command     : write_verilog -force -mode synth_stub
//               /home/f4bjh/devel/Rf-Test-BJH/tests/hf_gen_fpga/Cmod_A7-hf_gen_fpga/Cmod_A7-hf_gen_fpga.gen/sources_1/bd/design_1/ip/design_1_Stimu2_0_0/design_1_Stimu2_0_0_stub.v
// Design      : design_1_Stimu2_0_0
// Purpose     : Stub declaration of top-level module interface
// Device      : xc7a35tcpg236-1
// --------------------------------------------------------------------------------

// This empty module with port declaration file causes synthesis tools to infer a black box for IP.
// The synthesis directives are for Synopsys Synplify support to prevent IO buffer insertion.
// Please paste the declaration into a Verilog source file or add the file as an additional source.
(* CHECK_LICENSE_TYPE = "design_1_Stimu2_0_0,Stimu2,{}" *) (* CORE_GENERATION_INFO = "design_1_Stimu2_0_0,Stimu2,{x_ipProduct=Vivado 2024.2,x_ipVendor=xilinx.com,x_ipLibrary=module_ref,x_ipName=Stimu2,x_ipVersion=1.0,x_ipCoreRevision=1,x_ipLanguage=VERILOG,x_ipSimLanguage=MIXED}" *) (* DowngradeIPIdentifiedWarnings = "yes" *) 
(* IP_DEFINITION_SOURCE = "module_ref" *) (* X_CORE_INFO = "Stimu2,Vivado 2024.2" *) 
module design_1_Stimu2_0_0(tb_clock12M, tb_reset)
/* synthesis syn_black_box black_box_pad_pin="tb_clock12M,tb_reset" */;
  output tb_clock12M;
  (* X_INTERFACE_INFO = "xilinx.com:signal:reset:1.0 tb_reset RST" *) (* X_INTERFACE_MODE = "master" *) (* X_INTERFACE_PARAMETER = "XIL_INTERFACENAME tb_reset, POLARITY ACTIVE_LOW, INSERT_VIP 0" *) output tb_reset;
endmodule
