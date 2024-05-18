# Rf-Test-BJH
RF test bench project for amateur-radio application

- RF signal generator (4-5GHz)
- Frequency counter
- RF signal power meter

Based on ESP32 hardware



- trics and tips 

$ . $HOME/esp/esp-idf/export.sh

$ idf.py set-target esp32s3
$ idf.py menuconfig
$ idf.py build

~/esp/esp-idf$ openocd -f board/esp32s3-builtin.cfg
