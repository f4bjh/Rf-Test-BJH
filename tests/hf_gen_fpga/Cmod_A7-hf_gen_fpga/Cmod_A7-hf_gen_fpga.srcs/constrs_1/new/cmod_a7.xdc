## Clock input (12 MHz oscillator on Cmod A7)
set_property PACKAGE_PIN L17 [get_ports clock12M]
set_property IOSTANDARD LVCMOS33 [get_ports clock12M]
create_clock -period 83.333 -name sys_clk -waveform {0 41.666} [get_ports clock12M]

set_property PACKAGE_PIN G17 [get_ports clk]
set_property IOSTANDARD LVCMOS33 [get_ports clk]
set_property PACKAGE_PIN G19 [get_ports clkn]
set_property IOSTANDARD LVCMOS33 [get_ports clkn]
set_property PACKAGE_PIN N18 [get_ports reset_n]
set_property IOSTANDARD LVCMOS33 [get_ports reset_n]