transcript off
onbreak {quit -force}
onerror {quit -force}
transcript on

vlib work
vlib riviera/xpm
vlib riviera/xil_defaultlib

vmap xpm riviera/xpm
vmap xil_defaultlib riviera/xil_defaultlib

vlog -work xpm  -incr -l xpm -l xil_defaultlib \
"/home/f4bjh/Xilinx/Vivado/2024.2/data/ip/xpm/xpm_cdc/hdl/xpm_cdc.sv" \

vcom -work xpm -93  -incr \
"/home/f4bjh/Xilinx/Vivado/2024.2/data/ip/xpm/xpm_VCOMP.vhd" \

vlog -work xil_defaultlib  -incr -v2k5 -l xpm -l xil_defaultlib \
"../../../bd/design_1/ip/design_1_oscillator2_0_0/sim/design_1_oscillator2_0_0.v" \
"../../../bd/design_1/ip/design_1_Stimu2_0_0/sim/design_1_Stimu2_0_0.v" \
"../../../bd/design_1/sim/design_1.v" \

vlog -work xil_defaultlib \
"glbl.v"

