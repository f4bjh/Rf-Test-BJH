#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include <esp_ota_ops.h>
#include <nvs_flash.h>
#include "esp_log.h"
#include <string.h>
#include <driver/spi_master.h>

#include "http_server.h"
#include "wifi.h"
#include "lcd.h"
#include "spi.h"
#include "gpio.h"

#define BIT_0 (1<<0)
#define BIT_1 (1<<1)
#define BIT_2 (1<<2)
#define BIT_3 (1<<3)
#define BIT_4 (1<<4)
#define BIT_5 (1<<5)
#define BIT_6 (1<<6)
#define BIT_7 (1<<7)
