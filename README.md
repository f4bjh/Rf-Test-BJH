# Rf-Test-BJH
RF test bench project for amateur-radio application

- RF signal generator (4-5GHz)
- Frequency counter
- RF signal power meter

Based on ESP32 hardware



- trics and tips 

```
$ . $HOME/esp/esp-idf/export.sh
```

the very 1st time, when creating a new projet  
```
$ idf.py set-target esp32s3    
$ idf.py menuconfig  

```

update esp-idf environnement to a release:  
```
~/esp/esp-idf$  git fetch origin
~/esp/esp-idf$  git tag -l
~/esp/esp-idf$  git checkout tags/v5.4
~/esp/esp-idf$  git submodule update --init --recursive
~/esp/esp-idf$  ./install.sh esp32s3
~/devel/Rf-Test-BJH$  . $HOME/esp/esp-idf/export.sh
```

generate a savec version of sdkconfig  
```
$ idf.py save-defconfig
```

build and flash :  
```
$ idf.py build  
$ idf.py -p /dev/ttyACM0 flash monitor
```

make clean : 
```
$ idf.py fullclean
```

with minicom:  
```
$ minicom -D /dev/ttyACM0 
``` 

- gdb

```
~/esp/esp-idf$ openocd -f board/esp32s3-builtin.cfg  
~/devel/Rf-Test-BJH$ xtensa-esp32-elf-gdb -x ../gdbinit build/Rf-Test-BJH.elf  
```

reset and halt at start in gdb:  
```
(gdb) monitor reset halt
``` 

print string:  
```
(gdb) printf "%s", pointer_to_string
```

- menuconfig

```
$ idf.py menuconfig  

```


Set log level verbosity  


`component config -> log output`
default log verbosity = <choose level>  
maximum log verbosity = same as default  



- troubleshooting

cas d'erreur 1 :  
`ModuleNotFoundError: No module named 'common_test_methods'`
`ModuleNotFoundError: No module named 'pytest'`

$ cd ~/esp/esp-idf
$ . ./install.sh --enable-pytest
$ . ./export.sh

cas d'erreur 2 : 

`ModuleNotFoundError: No module named 'common_test_methods'`

apply this patch to the python script : 
```
try:
    from common_test_methods import get_env_config_variable
except ModuleNotFoundError:
    idf_path = os.environ['IDF_PATH']
    sys.path.insert(0, idf_path + '/tools/ci/python_packages')
    from common_test_methods import get_env_config_variable
```

cas d'erreur 3 :  
`ModuleNotFoundError: No module named 'websocket'`
solution : `$ python3 -m pip install websocket-client`

cas d'erreur 4: openocd
`Error: esp_usb_jtag: could not find or open device!`
solution : plug the USB cable to the other USB oprt of ESP32 board (JATG)

cas d'erreur 5: web pages are not updated after compilation
strange behavior of the compilator so far. Need to clean the project :  
```
$ idf.py clean
$ idf.py build
```

