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
