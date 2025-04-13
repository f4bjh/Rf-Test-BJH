#include <freertos/FreeRTOS.h>
#include <esp_http_server.h>
#include <freertos/task.h>
#include <esp_ota_ops.h>
#include "esp_log.h"
#include <sys/param.h>

#include "meas_mgt.h"
#include "meas.h"

esp_err_t get_current_part(meas_t *measure)
{
    const esp_partition_t *partition; 

    memset(measure->pdata, 0, measure->size);
        
    partition = esp_ota_get_running_partition();
    measure->size = MIN(strlen(partition->label)+1,7);
    memcpy(measure->pdata, (uint8_t*)partition->label, measure->size);
    measure->ready=true;

    return ESP_OK;
    
}

esp_err_t init_current_part(meas_t *measure)
{
    measure->size = 7*sizeof(char);
    measure->pdata = malloc(measure->size * sizeof(uint8_t));
    measure->pdata_cache = malloc(measure->size * sizeof(uint8_t));
    measure->meas_func = get_current_part;
    measure->handle = NULL;

    return ESP_OK;

}

esp_err_t calc_current_part(instance_meas_t *instance_meas)
{
    meas_t measure=instance_meas->measures;
    memset(instance_meas->calc_value, 0, CALC_VALUE_SIZE*sizeof(char));
 
    snprintf(instance_meas->calc_value,measure.size,"%s", measure.pdata_cache);

    return ESP_OK;
}

esp_err_t get_next_part(meas_t *measure)
{
    const esp_partition_t *partition; 

    memset(measure->pdata, 0, measure->size);
        
    partition = esp_ota_get_next_update_partition(NULL);
    measure->size = MIN(strlen(partition->label)+1,7);
    memcpy(measure->pdata, (uint8_t*)partition->label, measure->size);
    measure->ready=true;

    return ESP_OK;
    
}

esp_err_t init_next_part(meas_t *measure)
{
    measure->size = 7*sizeof(char);
    measure->pdata = malloc(measure->size * sizeof(uint8_t));
    measure->pdata_cache = malloc(measure->size * sizeof(uint8_t));
    measure->meas_func = get_next_part;
    measure->handle = NULL;

    return ESP_OK;

}

esp_err_t calc_next_part(instance_meas_t *instance_meas)
{
    meas_t measure=instance_meas->measures;
    memset(instance_meas->calc_value, 0, CALC_VALUE_SIZE*sizeof(char));
 
    snprintf(instance_meas->calc_value,measure.size,"%s", measure.pdata_cache);

    return ESP_OK;
}

esp_err_t get_current_part_version(meas_t *measure)
{
    const esp_partition_t *partition; 
    esp_app_desc_t app_desc;
    esp_err_t ret;

    memset(measure->pdata, 0, measure->size);
        
    partition = esp_ota_get_running_partition();
    ret = esp_ota_get_partition_description(partition, &app_desc);

    if (ret == ESP_OK){
      measure->size = MIN(strlen(app_desc.version),32); 
      memcpy(measure->pdata, (uint8_t*)app_desc.version, measure->size);
      measure->ready=true;
    } else
      measure->ready=false;

    return ret;
    
}

esp_err_t init_current_part_version(meas_t *measure)
{
    measure->size = 32*sizeof(char);
    measure->pdata = malloc(measure->size * sizeof(uint8_t));
    measure->pdata_cache = malloc(measure->size * sizeof(uint8_t));
    measure->meas_func = get_current_part_version;
    measure->handle = NULL;

    return ESP_OK;

}

esp_err_t calc_current_part_version(instance_meas_t *instance_meas)
{
    meas_t measure=instance_meas->measures;
    memset(instance_meas->calc_value, 0, CALC_VALUE_SIZE*sizeof(char));
 
    snprintf(instance_meas->calc_value,measure.size,"%s", measure.pdata_cache);

    return ESP_OK;
}

esp_err_t get_current_part_build_date(meas_t *measure)
{
    const esp_partition_t *partition; 
    esp_app_desc_t app_desc;
    esp_err_t ret;

    memset(measure->pdata, 0, measure->size);
        
    partition = esp_ota_get_running_partition();
    ret = esp_ota_get_partition_description(partition, &app_desc);

    if (ret == ESP_OK) {
      measure->size = 32; //just confirmed so far
      memcpy(measure->pdata, (uint8_t*)app_desc.date, 16);
      memcpy(measure->pdata + 16, (uint8_t*)app_desc.time, 16);
      measure->ready=true;
    } else
      measure->ready=false;

    return ret;
    
}

esp_err_t init_current_part_build_date(meas_t *measure)
{
    measure->size = (16 + 16)*sizeof(char);
    measure->pdata = malloc(measure->size * sizeof(uint8_t));
    measure->pdata_cache = malloc(measure->size * sizeof(uint8_t));
    measure->meas_func = get_current_part_build_date;
    measure->handle = NULL;

    return ESP_OK;

}

esp_err_t calc_current_part_build_date(instance_meas_t *instance_meas)
{
    meas_t measure=instance_meas->measures;
    int n;
    memset(instance_meas->calc_value, 0, CALC_VALUE_SIZE*sizeof(char));
 
    n= snprintf(instance_meas->calc_value,19, "%s - ", measure.pdata_cache);
    snprintf(instance_meas->calc_value + n,17,"%s", measure.pdata_cache + 16);


    return ESP_OK;
}

esp_err_t get_next_part_version(meas_t *measure)
{
    const esp_partition_t *partition; 
    esp_app_desc_t app_desc;
    esp_err_t ret;

    memset(measure->pdata, 0, measure->size);
        
    partition = esp_ota_get_next_update_partition(NULL);
    ret = esp_ota_get_partition_description(partition, &app_desc);

    if (ret == ESP_OK) {
      measure->size = MIN(strlen(app_desc.version)+1,32);
      memcpy(measure->pdata, (uint8_t*)app_desc.version, measure->size);
      measure->ready=true;
    } else
      measure->ready=false;

    return ret;
    
}

esp_err_t init_next_part_version(meas_t *measure)
{
    measure->size = 32*sizeof(char);
    measure->pdata = malloc(measure->size * sizeof(uint8_t));
    measure->pdata_cache = malloc(measure->size * sizeof(uint8_t));
    measure->meas_func = get_next_part_version;
    measure->handle = NULL;

    return ESP_OK;

}

esp_err_t calc_next_part_version(instance_meas_t *instance_meas)
{
    meas_t measure=instance_meas->measures;
    memset(instance_meas->calc_value, 0, CALC_VALUE_SIZE*sizeof(char));
 
    snprintf(instance_meas->calc_value,measure.size,"%s", measure.pdata_cache);

    return ESP_OK;
}

esp_err_t get_next_part_build_date(meas_t *measure)
{
    const esp_partition_t *partition; 
    esp_app_desc_t app_desc;
    esp_err_t ret;

    memset(measure->pdata, 0, measure->size);
        
    partition = esp_ota_get_next_update_partition(NULL);
    ret = esp_ota_get_partition_description(partition, &app_desc);

    if (ret == ESP_OK) {
      measure->size =32; //just confirmed so far
      memcpy(measure->pdata, (uint8_t*)app_desc.date, 16);
      memcpy(measure->pdata + 16, (uint8_t*)app_desc.time, 16);
      measure->ready=true;
   } else
      measure->ready=false;
    

    return ret;
    
}

esp_err_t init_next_part_build_date(meas_t *measure)
{
    measure->size = (16 + 16)*sizeof(char);
    measure->pdata = malloc(measure->size * sizeof(uint8_t));
    measure->pdata_cache = malloc(measure->size * sizeof(uint8_t));
    measure->meas_func = get_next_part_build_date;
    measure->handle = NULL;

    return ESP_OK;

}

esp_err_t calc_next_part_build_date(instance_meas_t *instance_meas)
{
    meas_t measure=instance_meas->measures;
    int n;
    memset(instance_meas->calc_value, 0, CALC_VALUE_SIZE*sizeof(char));
 
    n= snprintf(instance_meas->calc_value,19, "%s - ", measure.pdata_cache);
    snprintf(instance_meas->calc_value + n,16,"%s", measure.pdata_cache + 16);


    return ESP_OK;
}
