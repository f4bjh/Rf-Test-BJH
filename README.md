# Rf-Test-BJH

## Introduction

Rf-Test-BJH is an amateur radio project of a universal radio test bench.

## Features

 - ESP32 microntoller

 - Web server to display measures result

 - Wi-Fi connectivity  

 - Fast finite state machine for measurement acquisition  

 - LCD screen management  

 - RF generator
   - based on ADF4351  
   - Expected characteristics :
     - Pout = [-70dBm; 10dBm]
     - Pout step = 0,1dB
     - Frequency = 0-5GHz  

 - RF power meter
   - based on log detector AD8313
   - Expected characteristics :
     - Pin = [-70dBm; 10dBm]
     - Pout step = 0,1dB
     - Frequency = 0-5GHz

 - RF frequencymeter
   - based on S53MV design
   - Expected characteristics :
     - Frequency = 0-5GHz
     - Resolution : < 1hz

## User manuals : 

[How to setup wifi](./doc/Rf-Test-BJH/how-to/setup-wifi.md)

[How to upload firware](./doc/Rf-Test-BJH/how-to/update-fw.md)

[User guide for developpers](./doc/Rf-Test-BJH/how-to/for_developpers.md)

## Screenshot

![Index](./doc/Rf-Test-BJH/how-to/index.gif)

## Developpement documentation

[Firmware architecture](./doc/Rf-Test-BJH/archi/archi-sw-new2.drawio.svg)

[Partition table](./doc/Rf-Test-BJH/archi/partition-table.drawio.svg)

[Hardware architecture](./doc/Rf-Test-BJH/archi/hw-synoptique-3.drawio.svg)

[Frequencymeter architecture](./doc/Rf-Test-BJH/archi/rf-freq-archi.drawio.svg)

Note : in doc folder of this repo, you will find pdf version of this documentation
