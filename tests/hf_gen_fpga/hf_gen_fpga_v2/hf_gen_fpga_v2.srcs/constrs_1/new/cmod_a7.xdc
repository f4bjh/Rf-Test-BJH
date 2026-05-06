## Clock input (12 MHz oscillator on Cmod A7)
set_property PACKAGE_PIN L17 [get_ports clk_in]
set_property IOSTANDARD LVCMOS33 [get_ports clk_in]
create_clock -period 83.333 -name sys_clk -waveform {0 41.666} [get_ports clk_in]

set_property PACKAGE_PIN G17 [get_ports clk_out]
set_property IOSTANDARD LVCMOS33 [get_ports clk_out]
set_property PACKAGE_PIN N18 [get_ports reset_n]
set_property IOSTANDARD LVCMOS33 [get_ports reset_n]
set_property PACKAGE_PIN A18 [get_ports decr_push_button]
set_property IOSTANDARD LVCMOS33 [get_ports decr_push_button]