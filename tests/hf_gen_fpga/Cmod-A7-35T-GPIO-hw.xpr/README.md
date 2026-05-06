1 - Launch Vivado :   

```
$ source Xilinx/Vivado/2024.2/settings64.sh   
$ vivado &  
```

2- Generate bitstream  

2.1 : synthesis  
2.2 : Implementation  
2.3 : Generate Bitstream  

3 - Plug the Cmod A7 into the computer using the microUSB cable.  

4 - Open Hardware Manager and Program device  

5 - setup UART :  

$ minicom -D /dev/ttyUSB1  

configure minicom with the settings 9600 baud rate, 8 bits, one stop bit, and no parity bit.  
each time you press button, it is printed on UART  

