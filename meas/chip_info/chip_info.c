#include <freertos/FreeRTOS.h>
#include <esp_http_server.h>
#include <freertos/task.h>
#include <esp_ota_ops.h>
#include "esp_log.h"
#include "esp_chip_info.h"

#include "meas_mgt.h"
#include "meas.h"

//standalone task on cpu1
//should fill instance_meas->measures.pdata
esp_err_t get_chip_info_model(meas_t *measure)
{
    esp_chip_info_t chip_info;

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
    measure->task_handle = NULL;

    return ESP_OK;

}

esp_err_t calc_chip_info_model(instance_meas_t *instance_meas)
{
    meas_t measure=instance_meas->measures;
    memset(instance_meas->calc_value, 0, CALC_VALUE_SIZE*sizeof(char));
 
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


esp_err_t get_chip_info_revision(meas_t *measure)
{
    esp_chip_info_t chip_info;

    memset(measure->pdata, 0, measure->size);
        
    esp_chip_info(&chip_info);
    memcpy(measure->pdata, (uint8_t*) &(chip_info.revision), measure->size);
    measure->ready=true;

    return ESP_OK;
}

esp_err_t init_chip_revision(meas_t *measure)
{
    measure->size = sizeof(uint16_t);
    measure->pdata = malloc(measure->size * sizeof(uint8_t));
    measure->pdata_cache = malloc(measure->size * sizeof(uint8_t));
    measure->meas_func = get_chip_info_revision;
    measure->task_handle = NULL;

    return ESP_OK;

}


esp_err_t calc_chip_revision(instance_meas_t *instance_meas)
{
    meas_t measure=instance_meas->measures;
    memset(instance_meas->calc_value, 0, CALC_VALUE_SIZE*sizeof(char));
     
    unsigned major_rev = *(measure.pdata_cache) / 100;
    unsigned minor_rev = *(measure.pdata_cache) % 100;

    sprintf(instance_meas->calc_value,"%d.%d", major_rev, minor_rev);
   
    return ESP_OK;
}



