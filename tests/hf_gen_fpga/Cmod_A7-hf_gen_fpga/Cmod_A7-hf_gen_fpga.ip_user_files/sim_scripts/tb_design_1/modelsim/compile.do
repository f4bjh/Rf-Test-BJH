vlib modelsim_lib/work
vlib modelsim_lib/msim

vlib modelsim_lib/msim/xpm
vlib modelsim_lib/msim/xil_defaultlib

vmap xpm modelsim_lib/msim/xpm
vmap xil_defaultlib modelsim_lib/msim/xil_defaultlib

vlog -work xpm -64 -incr -mfcu  -sv \
"/home/f4bjh/Xilinx/Vivado/2024.2/data/ip/xpm/xpm_cdc/hdl/xpm_cdc.sv" \

vcom -work xpm -64 -93  \
"/home/f4bjh/Xilinx/Vivado/2024.2/data/ip/xpm/xpm_VCOMP.vhd" \

vlog -work xil_defaultlib -64 -incr -mfcu  \
"../../../bd/tb_design_1/ip/tb_design_1_Stimu_0_0/sim/tb_design_1_Stimu_0_0.v" \
"../../../bd/tb_design_1/ip/tb_design_1_hf_gen_fpga_0_0/sim/tb_design_1_hf_gen_fpga_0_0.v" \
"../../../bd/tb_design_1/sim/tb_design_1.v" \

vlog -work xil_defaultlib \
"glbl.v"

