# Rf-Test-BJH
RF test bench project for amateur-radio application

- RF signal generator (4-5GHz)
- Frequency counter
- RF signal power meter

Based on ESP32 hardware



- trics and tips 
```

$ . $HOME/esp/esp-idf/export.sh

$ idf.py set-target esp32s3    
$ idf.py menuconfig  
$ idf.py build  

~/devel/Rf-Test-BJH$ xtensa-esp32-elf-gdb -x ../gdbinit build/Rf-Test-BJH.elf  
$ minicom -D /dev/ttyACM0 
 
```

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

cas d'erreur 2 :  
`ModuleNotFoundError: No module named 'websocket'`
solution : `$ python3 -m pip install websocket-client`
