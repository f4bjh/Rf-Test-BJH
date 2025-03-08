#include <freertos/FreeRTOS.h>
#include <esp_http_server.h>
#include <freertos/task.h>
#include <esp_ota_ops.h>
#include "esp_log.h"
#include "esp_chip_info.h"

#include "meas_mgt.h"

//standalone task on cpu1
//should fill instance_meas->measures.pdata
esp_err_t get_chip_info_model(meas_t *measure)
{
    esp_chip_info_t chip_info;

    //todo : management once case    
    memset(measure->pdata, 0, measure->size);
        
    esp_chip_info(&chip_info);
    memcpy(measure->pdata, (uint8_t*) &(chip_info.model), measure->size);
    measure->ready=true;

    return ESP_OK;
    
}

esp_err_t init_chip_info_model(meas_t *measure)
{
    measure->size = sizeof(esp_chip_model_t);
    measure->pdata = malloc(measure->size * sizeof(uint8_t));
    measure->pdata_cache = malloc(measure->size * sizeof(uint8_t));
    measure->meas_func = get_chip_info_model;

    return ESP_OK;

}

esp_err_t calc_chip_info_model(instance_meas_t *instance_meas)
{
    meas_t measure=instance_meas->measures;


    switch((esp_chip_model_t) *measure.pdata_cache) {
      case CHIP_ESP32:
        sprintf(instance_meas->calc_value,"ESP32");
        break;
      case CHIP_ESP32S2:
        sprintf(instance_meas->calc_value,"ESP32-S2");
        break;
      case CHIP_ESP32S3:
        sprintf(instance_meas->calc_value,"ESP32-S3");
        break;
      case CHIP_ESP32C3:
        sprintf(instance_meas->calc_value,"ESP32-C3");
        break;
      case CHIP_ESP32C2:
        sprintf(instance_meas->calc_value,"ESP32-C2");
        break;
      case CHIP_ESP32C6:
        sprintf(instance_meas->calc_value,"ESP32-C6");
        break;
      case CHIP_ESP32H2:
        sprintf(instance_meas->calc_value,"ESP32-H2");
        break;
      case CHIP_ESP32P4:
        sprintf(instance_meas->calc_value,"ESP32-P4");
        break;
      case CHIP_POSIX_LINUX:
      default:
        sprintf(instance_meas->calc_value,"unknown");
        break;
    }
 
    return ESP_OK;
}


void get_chip_info_revision(char *data)
{

    esp_chip_info_t chip_info;
    //uint32_t flash_size;
    esp_chip_info(&chip_info);

    unsigned major_rev = chip_info.revision / 100;
    unsigned minor_rev = chip_info.revision % 100;

    memset(data, 0, 32*sizeof(char));
    sprintf(data,"%d.%d", major_rev, minor_rev);

}
