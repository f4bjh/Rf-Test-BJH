// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// Copyright 2022-2024 Advanced Micro Devices, Inc. All Rights Reserved.
// --------------------------------------------------------------------------------
// Tool Version: Vivado v.2024.2 (lin64) Build 5239630 Fri Nov 08 22:34:34 MST 2024
// Date        : Wed Sep 10 22:56:12 2025
// Host        : f4bjh-minipc running 64-bit Ubuntu 24.04.3 LTS
// Command     : write_verilog -force -mode funcsim
//               /home/f4bjh/devel/Rf-Test-BJH/tests/hf_gen_fpga/Cmod_A7-hf_gen_fpga/Cmod_A7-hf_gen_fpga.gen/sources_1/bd/tb_design_1/ip/tb_design_1_hf_gen_fpga_0_0/tb_design_1_hf_gen_fpga_0_0_sim_netlist.v
// Design      : tb_design_1_hf_gen_fpga_0_0
// Purpose     : This verilog netlist is a functional simulation representation of the design and should not be modified
//               or synthesized. This netlist cannot be used for SDF annotated simulation.
// Device      : xc7a35tcpg236-1
// --------------------------------------------------------------------------------
`timescale 1 ps / 1 ps

(* CHECK_LICENSE_TYPE = "tb_design_1_hf_gen_fpga_0_0,hf_gen_fpga,{}" *) (* DowngradeIPIdentifiedWarnings = "yes" *) (* IP_DEFINITION_SOURCE = "module_ref" *) 
(* X_CORE_INFO = "hf_gen_fpga,Vivado 2024.2" *) 
(* NotValidForBitStream *)
module tb_design_1_hf_gen_fpga_0_0
   (clk_pin,
    reset,
    clk,
    clkn);
  input clk_pin;
  (* X_INTERFACE_INFO = "xilinx.com:signal:reset:1.0 reset RST" *) (* X_INTERFACE_MODE = "slave" *) (* X_INTERFACE_PARAMETER = "XIL_INTERFACENAME reset, POLARITY ACTIVE_LOW, INSERT_VIP 0" *) input reset;
  (* X_INTERFACE_INFO = "xilinx.com:signal:clock:1.0 clk CLK" *) (* X_INTERFACE_MODE = "master" *) (* X_INTERFACE_PARAMETER = "XIL_INTERFACENAME clk, ASSOCIATED_RESET reset, FREQ_HZ 100000000, FREQ_TOLERANCE_HZ 0, PHASE 0.0, CLK_DOMAIN tb_design_1_hf_gen_fpga_0_0_clk, INSERT_VIP 0" *) output clk;
  output clkn;

  wire clk;
  wire clk_pin;
  wire clkn;
  wire reset;

  (* MAX = "8'b01000000" *) 
  tb_design_1_hf_gen_fpga_0_0_hf_gen_fpga inst
       (.clk(clk),
        .clk_pin(clk_pin),
        .clkn(clkn),
        .reset(reset));
endmodule

(* ORIG_REF_NAME = "clk_wiz_0" *) 
module tb_design_1_hf_gen_fpga_0_0_clk_wiz_0
   (clk_out1,
    reset,
    locked,
    clk_in1);
  (* syn_isclock = "1" *) output clk_out1;
  input reset;
  output locked;
  input clk_in1;


endmodule

(* MAX = "8'b01000000" *) (* ORIG_REF_NAME = "hf_gen_fpga" *) 
module tb_design_1_hf_gen_fpga_0_0_hf_gen_fpga
   (clk_pin,
    reset,
    clk,
    clkn);
  input clk_pin;
  input reset;
  output clk;
  output clkn;

  wire clk;
  wire clk_pin;
  wire clk_sig;
  wire clk_sig_d;
  wire clk_sig_i_1_n_0;
  wire clkn;
  wire clkn_sig;
  wire clkn_sig_i_1_n_0;
  wire clock50M;
  wire [15:0]counter;
  wire \counter[0]_i_1_n_0 ;
  wire \counter[0]_i_2_n_0 ;
  wire \counter[10]_i_1_n_0 ;
  wire \counter[11]_i_1_n_0 ;
  wire \counter[12]_i_1_n_0 ;
  wire \counter[13]_i_1_n_0 ;
  wire \counter[14]_i_1_n_0 ;
  wire \counter[15]_i_2_n_0 ;
  wire \counter[15]_i_3_n_0 ;
  wire \counter[15]_i_4_n_0 ;
  wire \counter[15]_i_6_n_0 ;
  wire \counter[15]_i_7_n_0 ;
  wire \counter[15]_i_8_n_0 ;
  wire \counter[15]_i_9_n_0 ;
  wire \counter[1]_i_1_n_0 ;
  wire \counter[2]_i_1_n_0 ;
  wire \counter[3]_i_1_n_0 ;
  wire \counter[4]_i_1_n_0 ;
  wire \counter[5]_i_1_n_0 ;
  wire \counter[6]_i_1_n_0 ;
  wire \counter[7]_i_1_n_0 ;
  wire \counter[8]_i_1_n_0 ;
  wire \counter[9]_i_1_n_0 ;
  wire counter_d;
  wire \counter_reg[12]_i_2_n_0 ;
  wire \counter_reg[12]_i_2_n_1 ;
  wire \counter_reg[12]_i_2_n_2 ;
  wire \counter_reg[12]_i_2_n_3 ;
  wire \counter_reg[15]_i_5_n_2 ;
  wire \counter_reg[15]_i_5_n_3 ;
  wire \counter_reg[4]_i_2_n_0 ;
  wire \counter_reg[4]_i_2_n_1 ;
  wire \counter_reg[4]_i_2_n_2 ;
  wire \counter_reg[4]_i_2_n_3 ;
  wire \counter_reg[8]_i_2_n_0 ;
  wire \counter_reg[8]_i_2_n_1 ;
  wire \counter_reg[8]_i_2_n_2 ;
  wire \counter_reg[8]_i_2_n_3 ;
  wire [15:1]data0;
  wire interlock;
  wire reset;
  wire [3:2]\NLW_counter_reg[15]_i_5_CO_UNCONNECTED ;
  wire [3:3]\NLW_counter_reg[15]_i_5_O_UNCONNECTED ;
  wire NLW_u_clk_locked_UNCONNECTED;

  (* SOFT_HLUTNM = "soft_lutpair0" *) 
  LUT2 #(
    .INIT(4'h2)) 
    clk_INST_0
       (.I0(clk_sig),
        .I1(interlock),
        .O(clk));
  (* SOFT_HLUTNM = "soft_lutpair8" *) 
  LUT2 #(
    .INIT(4'h9)) 
    clk_sig_i_1
       (.I0(\counter[15]_i_4_n_0 ),
        .I1(clk_sig),
        .O(clk_sig_i_1_n_0));
  FDCE clk_sig_reg
       (.C(clock50M),
        .CE(1'b1),
        .CLR(\counter[15]_i_3_n_0 ),
        .D(clk_sig_i_1_n_0),
        .Q(clk_sig));
  LUT2 #(
    .INIT(4'h2)) 
    clkn_INST_0
       (.I0(clkn_sig),
        .I1(interlock),
        .O(clkn));
  (* SOFT_HLUTNM = "soft_lutpair0" *) 
  LUT3 #(
    .INIT(8'hE2)) 
    clkn_sig_i_1
       (.I0(clk_sig),
        .I1(\counter[15]_i_4_n_0 ),
        .I2(clkn_sig),
        .O(clkn_sig_i_1_n_0));
  FDPE clkn_sig_reg
       (.C(clock50M),
        .CE(1'b1),
        .D(clkn_sig_i_1_n_0),
        .PRE(\counter[15]_i_3_n_0 ),
        .Q(clkn_sig));
  LUT2 #(
    .INIT(4'h2)) 
    \counter[0]_i_1 
       (.I0(\counter[0]_i_2_n_0 ),
        .I1(counter[0]),
        .O(\counter[0]_i_1_n_0 ));
  LUT6 #(
    .INIT(64'hFFFFFFFFFFFFFFFE)) 
    \counter[0]_i_2 
       (.I0(\counter[15]_i_9_n_0 ),
        .I1(\counter[15]_i_8_n_0 ),
        .I2(counter[15]),
        .I3(counter[14]),
        .I4(counter[1]),
        .I5(\counter[15]_i_6_n_0 ),
        .O(\counter[0]_i_2_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair2" *) 
  LUT2 #(
    .INIT(4'h8)) 
    \counter[10]_i_1 
       (.I0(\counter[15]_i_4_n_0 ),
        .I1(data0[10]),
        .O(\counter[10]_i_1_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair3" *) 
  LUT2 #(
    .INIT(4'h8)) 
    \counter[11]_i_1 
       (.I0(\counter[15]_i_4_n_0 ),
        .I1(data0[11]),
        .O(\counter[11]_i_1_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair3" *) 
  LUT2 #(
    .INIT(4'h8)) 
    \counter[12]_i_1 
       (.I0(\counter[15]_i_4_n_0 ),
        .I1(data0[12]),
        .O(\counter[12]_i_1_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair2" *) 
  LUT2 #(
    .INIT(4'h8)) 
    \counter[13]_i_1 
       (.I0(\counter[15]_i_4_n_0 ),
        .I1(data0[13]),
        .O(\counter[13]_i_1_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair1" *) 
  LUT2 #(
    .INIT(4'h8)) 
    \counter[14]_i_1 
       (.I0(\counter[15]_i_4_n_0 ),
        .I1(data0[14]),
        .O(\counter[14]_i_1_n_0 ));
  LUT2 #(
    .INIT(4'h7)) 
    \counter[15]_i_1 
       (.I0(\counter[15]_i_4_n_0 ),
        .I1(interlock),
        .O(counter_d));
  (* SOFT_HLUTNM = "soft_lutpair1" *) 
  LUT2 #(
    .INIT(4'h8)) 
    \counter[15]_i_2 
       (.I0(\counter[15]_i_4_n_0 ),
        .I1(data0[15]),
        .O(\counter[15]_i_2_n_0 ));
  LUT1 #(
    .INIT(2'h1)) 
    \counter[15]_i_3 
       (.I0(reset),
        .O(\counter[15]_i_3_n_0 ));
  LUT5 #(
    .INIT(32'hFFFFFFFE)) 
    \counter[15]_i_4 
       (.I0(\counter[15]_i_6_n_0 ),
        .I1(\counter[15]_i_7_n_0 ),
        .I2(\counter[15]_i_8_n_0 ),
        .I3(\counter[15]_i_9_n_0 ),
        .I4(counter[0]),
        .O(\counter[15]_i_4_n_0 ));
  LUT4 #(
    .INIT(16'hFFFE)) 
    \counter[15]_i_6 
       (.I0(counter[11]),
        .I1(counter[10]),
        .I2(counter[13]),
        .I3(counter[12]),
        .O(\counter[15]_i_6_n_0 ));
  LUT3 #(
    .INIT(8'hFE)) 
    \counter[15]_i_7 
       (.I0(counter[15]),
        .I1(counter[14]),
        .I2(counter[1]),
        .O(\counter[15]_i_7_n_0 ));
  LUT4 #(
    .INIT(16'hFFFE)) 
    \counter[15]_i_8 
       (.I0(counter[3]),
        .I1(counter[2]),
        .I2(counter[5]),
        .I3(counter[4]),
        .O(\counter[15]_i_8_n_0 ));
  LUT4 #(
    .INIT(16'hFFFD)) 
    \counter[15]_i_9 
       (.I0(counter[6]),
        .I1(counter[7]),
        .I2(counter[9]),
        .I3(counter[8]),
        .O(\counter[15]_i_9_n_0 ));
  LUT2 #(
    .INIT(4'h8)) 
    \counter[1]_i_1 
       (.I0(\counter[15]_i_4_n_0 ),
        .I1(data0[1]),
        .O(\counter[1]_i_1_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair7" *) 
  LUT2 #(
    .INIT(4'h8)) 
    \counter[2]_i_1 
       (.I0(\counter[15]_i_4_n_0 ),
        .I1(data0[2]),
        .O(\counter[2]_i_1_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair7" *) 
  LUT2 #(
    .INIT(4'h8)) 
    \counter[3]_i_1 
       (.I0(\counter[15]_i_4_n_0 ),
        .I1(data0[3]),
        .O(\counter[3]_i_1_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair6" *) 
  LUT2 #(
    .INIT(4'h8)) 
    \counter[4]_i_1 
       (.I0(\counter[15]_i_4_n_0 ),
        .I1(data0[4]),
        .O(\counter[4]_i_1_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair6" *) 
  LUT2 #(
    .INIT(4'h8)) 
    \counter[5]_i_1 
       (.I0(\counter[15]_i_4_n_0 ),
        .I1(data0[5]),
        .O(\counter[5]_i_1_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair4" *) 
  LUT2 #(
    .INIT(4'h8)) 
    \counter[6]_i_1 
       (.I0(\counter[15]_i_4_n_0 ),
        .I1(data0[6]),
        .O(\counter[6]_i_1_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair5" *) 
  LUT2 #(
    .INIT(4'h8)) 
    \counter[7]_i_1 
       (.I0(\counter[15]_i_4_n_0 ),
        .I1(data0[7]),
        .O(\counter[7]_i_1_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair5" *) 
  LUT2 #(
    .INIT(4'h8)) 
    \counter[8]_i_1 
       (.I0(\counter[15]_i_4_n_0 ),
        .I1(data0[8]),
        .O(\counter[8]_i_1_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair4" *) 
  LUT2 #(
    .INIT(4'h8)) 
    \counter[9]_i_1 
       (.I0(\counter[15]_i_4_n_0 ),
        .I1(data0[9]),
        .O(\counter[9]_i_1_n_0 ));
  FDCE \counter_reg[0] 
       (.C(clock50M),
        .CE(counter_d),
        .CLR(\counter[15]_i_3_n_0 ),
        .D(\counter[0]_i_1_n_0 ),
        .Q(counter[0]));
  FDCE \counter_reg[10] 
       (.C(clock50M),
        .CE(counter_d),
        .CLR(\counter[15]_i_3_n_0 ),
        .D(\counter[10]_i_1_n_0 ),
        .Q(counter[10]));
  FDCE \counter_reg[11] 
       (.C(clock50M),
        .CE(counter_d),
        .CLR(\counter[15]_i_3_n_0 ),
        .D(\counter[11]_i_1_n_0 ),
        .Q(counter[11]));
  FDCE \counter_reg[12] 
       (.C(clock50M),
        .CE(counter_d),
        .CLR(\counter[15]_i_3_n_0 ),
        .D(\counter[12]_i_1_n_0 ),
        .Q(counter[12]));
  (* ADDER_THRESHOLD = "35" *) 
  CARRY4 \counter_reg[12]_i_2 
       (.CI(\counter_reg[8]_i_2_n_0 ),
        .CO({\counter_reg[12]_i_2_n_0 ,\counter_reg[12]_i_2_n_1 ,\counter_reg[12]_i_2_n_2 ,\counter_reg[12]_i_2_n_3 }),
        .CYINIT(1'b0),
        .DI({1'b0,1'b0,1'b0,1'b0}),
        .O(data0[12:9]),
        .S(counter[12:9]));
  FDCE \counter_reg[13] 
       (.C(clock50M),
        .CE(counter_d),
        .CLR(\counter[15]_i_3_n_0 ),
        .D(\counter[13]_i_1_n_0 ),
        .Q(counter[13]));
  FDCE \counter_reg[14] 
       (.C(clock50M),
        .CE(counter_d),
        .CLR(\counter[15]_i_3_n_0 ),
        .D(\counter[14]_i_1_n_0 ),
        .Q(counter[14]));
  FDCE \counter_reg[15] 
       (.C(clock50M),
        .CE(counter_d),
        .CLR(\counter[15]_i_3_n_0 ),
        .D(\counter[15]_i_2_n_0 ),
        .Q(counter[15]));
  (* ADDER_THRESHOLD = "35" *) 
  CARRY4 \counter_reg[15]_i_5 
       (.CI(\counter_reg[12]_i_2_n_0 ),
        .CO({\NLW_counter_reg[15]_i_5_CO_UNCONNECTED [3:2],\counter_reg[15]_i_5_n_2 ,\counter_reg[15]_i_5_n_3 }),
        .CYINIT(1'b0),
        .DI({1'b0,1'b0,1'b0,1'b0}),
        .O({\NLW_counter_reg[15]_i_5_O_UNCONNECTED [3],data0[15:13]}),
        .S({1'b0,counter[15:13]}));
  FDCE \counter_reg[1] 
       (.C(clock50M),
        .CE(counter_d),
        .CLR(\counter[15]_i_3_n_0 ),
        .D(\counter[1]_i_1_n_0 ),
        .Q(counter[1]));
  FDCE \counter_reg[2] 
       (.C(clock50M),
        .CE(counter_d),
        .CLR(\counter[15]_i_3_n_0 ),
        .D(\counter[2]_i_1_n_0 ),
        .Q(counter[2]));
  FDCE \counter_reg[3] 
       (.C(clock50M),
        .CE(counter_d),
        .CLR(\counter[15]_i_3_n_0 ),
        .D(\counter[3]_i_1_n_0 ),
        .Q(counter[3]));
  FDCE \counter_reg[4] 
       (.C(clock50M),
        .CE(counter_d),
        .CLR(\counter[15]_i_3_n_0 ),
        .D(\counter[4]_i_1_n_0 ),
        .Q(counter[4]));
  (* ADDER_THRESHOLD = "35" *) 
  CARRY4 \counter_reg[4]_i_2 
       (.CI(1'b0),
        .CO({\counter_reg[4]_i_2_n_0 ,\counter_reg[4]_i_2_n_1 ,\counter_reg[4]_i_2_n_2 ,\counter_reg[4]_i_2_n_3 }),
        .CYINIT(counter[0]),
        .DI({1'b0,1'b0,1'b0,1'b0}),
        .O(data0[4:1]),
        .S(counter[4:1]));
  FDCE \counter_reg[5] 
       (.C(clock50M),
        .CE(counter_d),
        .CLR(\counter[15]_i_3_n_0 ),
        .D(\counter[5]_i_1_n_0 ),
        .Q(counter[5]));
  FDCE \counter_reg[6] 
       (.C(clock50M),
        .CE(counter_d),
        .CLR(\counter[15]_i_3_n_0 ),
        .D(\counter[6]_i_1_n_0 ),
        .Q(counter[6]));
  FDCE \counter_reg[7] 
       (.C(clock50M),
        .CE(counter_d),
        .CLR(\counter[15]_i_3_n_0 ),
        .D(\counter[7]_i_1_n_0 ),
        .Q(counter[7]));
  FDCE \counter_reg[8] 
       (.C(clock50M),
        .CE(counter_d),
        .CLR(\counter[15]_i_3_n_0 ),
        .D(\counter[8]_i_1_n_0 ),
        .Q(counter[8]));
  (* ADDER_THRESHOLD = "35" *) 
  CARRY4 \counter_reg[8]_i_2 
       (.CI(\counter_reg[4]_i_2_n_0 ),
        .CO({\counter_reg[8]_i_2_n_0 ,\counter_reg[8]_i_2_n_1 ,\counter_reg[8]_i_2_n_2 ,\counter_reg[8]_i_2_n_3 }),
        .CYINIT(1'b0),
        .DI({1'b0,1'b0,1'b0,1'b0}),
        .O(data0[8:5]),
        .S(counter[8:5]));
  FDCE \counter_reg[9] 
       (.C(clock50M),
        .CE(counter_d),
        .CLR(\counter[15]_i_3_n_0 ),
        .D(\counter[9]_i_1_n_0 ),
        .Q(counter[9]));
  (* SOFT_HLUTNM = "soft_lutpair8" *) 
  LUT1 #(
    .INIT(2'h1)) 
    interlock_i_1
       (.I0(\counter[15]_i_4_n_0 ),
        .O(clk_sig_d));
  FDCE interlock_reg
       (.C(clock50M),
        .CE(1'b1),
        .CLR(\counter[15]_i_3_n_0 ),
        .D(clk_sig_d),
        .Q(interlock));
  tb_design_1_hf_gen_fpga_0_0_clk_wiz_0 u_clk
       (.clk_in1(clk_pin),
        .clk_out1(clock50M),
        .locked(NLW_u_clk_locked_UNCONNECTED),
        .reset(1'b0));
endmodule
`ifndef GLBL
`define GLBL
`timescale  1 ps / 1 ps

module glbl ();

    parameter ROC_WIDTH = 100000;
    parameter TOC_WIDTH = 0;
    parameter GRES_WIDTH = 10000;
    parameter GRES_START = 10000;

//--------   STARTUP Globals --------------
    wire GSR;
    wire GTS;
    wire GWE;
    wire PRLD;
    wire GRESTORE;
    tri1 p_up_tmp;
    tri (weak1, strong0) PLL_LOCKG = p_up_tmp;

    wire PROGB_GLBL;
    wire CCLKO_GLBL;
    wire FCSBO_GLBL;
    wire [3:0] DO_GLBL;
    wire [3:0] DI_GLBL;
   
    reg GSR_int;
    reg GTS_int;
    reg PRLD_int;
    reg GRESTORE_int;

//--------   JTAG Globals --------------
    wire JTAG_TDO_GLBL;
    wire JTAG_TCK_GLBL;
    wire JTAG_TDI_GLBL;
    wire JTAG_TMS_GLBL;
    wire JTAG_TRST_GLBL;

    reg JTAG_CAPTURE_GLBL;
    reg JTAG_RESET_GLBL;
    reg JTAG_SHIFT_GLBL;
    reg JTAG_UPDATE_GLBL;
    reg JTAG_RUNTEST_GLBL;

    reg JTAG_SEL1_GLBL = 0;
    reg JTAG_SEL2_GLBL = 0 ;
    reg JTAG_SEL3_GLBL = 0;
    reg JTAG_SEL4_GLBL = 0;

    reg JTAG_USER_TDO1_GLBL = 1'bz;
    reg JTAG_USER_TDO2_GLBL = 1'bz;
    reg JTAG_USER_TDO3_GLBL = 1'bz;
    reg JTAG_USER_TDO4_GLBL = 1'bz;

    assign (strong1, weak0) GSR = GSR_int;
    assign (strong1, weak0) GTS = GTS_int;
    assign (weak1, weak0) PRLD = PRLD_int;
    assign (strong1, weak0) GRESTORE = GRESTORE_int;

    initial begin
	GSR_int = 1'b1;
	PRLD_int = 1'b1;
	#(ROC_WIDTH)
	GSR_int = 1'b0;
	PRLD_int = 1'b0;
    end

    initial begin
	GTS_int = 1'b1;
	#(TOC_WIDTH)
	GTS_int = 1'b0;
    end

    initial begin 
	GRESTORE_int = 1'b0;
	#(GRES_START);
	GRESTORE_int = 1'b1;
	#(GRES_WIDTH);
	GRESTORE_int = 1'b0;
    end

endmodule
`endif
