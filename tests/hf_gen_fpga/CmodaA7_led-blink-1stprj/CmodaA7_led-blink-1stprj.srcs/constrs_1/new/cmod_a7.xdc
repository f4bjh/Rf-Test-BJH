## Clock input (12 MHz oscillator on Cmod A7)
set_property PACKAGE_PIN L17 [get_ports clk_pin]
set_property IOSTANDARD LVCMOS33 [get_ports clk_pin]
create_clock -period 83.333 -name sys_clk -waveform {0 41.666} [get_ports clk_pin]

## LED0 (user LED on Cmod A7)
set_property PACKAGE_PIN A17 [get_ports led_o]
set_property IOSTANDARD LVCMOS33 [get_ports led_o]