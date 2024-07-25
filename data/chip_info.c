#include <freertos/FreeRTOS.h>
#include <esp_http_server.h>
#include <freertos/task.h>
#include <esp_ota_ops.h>
#include "esp_log.h"
#include "esp_chip_info.h"

#include "data.h"

void get_chip_info_model(char *data, char *length)
{
    esp_chip_info_t chip_info;
    //uint32_t flash_size;
    esp_chip_info(&chip_info);

    switch(chip_info.model) {
      case CHIP_ESP32:
        sprintf(data,"ESP32");
	*length = 5;
        break;
      case CHIP_ESP32S2:
        sprintf(data,"ESP32-S2");
	*length = 8;
        break;
      case CHIP_ESP32S3:
        sprintf(data,"ESP32-S3");
	*length = 8;
        break;
      case CHIP_ESP32C3:
        sprintf(data,"ESP32-C3");
	*length = 8;
        break;
      case CHIP_ESP32C2:
        sprintf(data,"ESP32-C2");
	*length = 8;
        break;
      case CHIP_ESP32C6:
        sprintf(data,"ESP32-C6");
	*length = 8;
        break;
      case CHIP_ESP32H2:
        sprintf(data,"ESP32-H2");
	*length = 8;
        break;
      case CHIP_ESP32P4:
        sprintf(data,"ESP32-P4");
	*length = 8;
        break;
      case CHIP_POSIX_LINUX:
      default:
        sprintf(data,"unknown");
	*length = 7;
        break;
    }
 
}


static void get_chip_info_revision(char *data)
{

    esp_chip_info_t chip_info;
    //uint32_t flash_size;
    esp_chip_info(&chip_info);

    unsigned major_rev = chip_info.revision / 100;
    unsigned minor_rev = chip_info.revision % 100;

    memset(data, 0, 32*sizeof(char));
    sprintf(data,"%d.%d", major_rev, minor_rev);

}
