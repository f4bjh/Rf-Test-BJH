#include <freertos/FreeRTOS.h>
#include <esp_http_server.h>
#include <freertos/task.h>
#include <esp_ota_ops.h>
#include "esp_log.h"

#include "meas_mgt.h"
#include "meas.h"

esp_err_t get_current_part(meas_t *measure)
{
    const esp_partition_t *partition; 

    memset(measure->pdata, 0, measure->size);
        
    partition = esp_ota_get_running_partition();
    memcpy(measure->pdata, (uint8_t*)partition->label, measure->size);
    measure->ready=true;

    return ESP_OK;
    
}

esp_err_t init_current_part(meas_t *measure)
{
    measure->size = 6*sizeof(char);
    measure->pdata = malloc(measure->size * sizeof(uint8_t));
    measure->pdata_cache = malloc(measure->size * sizeof(uint8_t));
    measure->meas_func = get_current_part;
    measure->task_handle = NULL;

    return ESP_OK;

}

esp_err_t calc_current_part(instance_meas_t *instance_meas)
{
    meas_t measure=instance_meas->measures;
    memset(instance_meas->calc_value, 0, CALC_VALUE_SIZE*sizeof(char));
 
    sprintf(instance_meas->calc_value,"%s", measure.pdata_cache);

    return ESP_OK;
}
