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

[How to setup wifi](./doc/setup-wifi.md)

[How to upload firware](./doc/update-fw.md)

[User guide for developpers](./doc/for_developpers.md)

## Screenshot

![Index](./doc/index.gif)

## Developpement documentation

[Firmware architecture](./doc/archi-sw-new.jpg)

[Partition table](./doc/partition-table.drawio.svg)

[Hardware architecture](./doc/hw-synoptique-3.drawio.svg)

[Frequencymeter architecture](./doc/rf-freq-archi.drawio.svg)

Note : in doc folder of this repo, you will find pdf version of this documentation

