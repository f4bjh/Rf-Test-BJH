#include <string.h>

#include <freertos/FreeRTOS.h>
#include <esp_http_server.h>
#include <freertos/task.h>
#include <esp_ota_ops.h>
#include "esp_log.h"

#include "cJSON.h"
#include "data.h"

static const char* TAG = "data";

QueueHandle_t xQueue;

void get_measurement(void *pvParameters)
{

    json_data_t json_data; 
    BaseType_t xStatus;

    json_data.value = malloc( 32*sizeof(char)); 
    memset(json_data.value, 0, 32*sizeof(char));

    //here we should add a switch case, base on a queue intertask data
    //in wich, we choose wich data should be updated
    get_chip_info_model(json_data.value, &json_data.length);

    if (json_data.length !=0) {

        json_data.tag = CHIP_INFO_MODEL_DATA_TAG;//should be the same value as in the switch case

    	cJSON *root = cJSON_CreateObject();
	cJSON_AddNumberToObject(root, "tag", json_data.tag);
    	cJSON_AddNumberToObject(root, "length", json_data.length);
    	cJSON_AddStringToObject(root, "value", json_data.value);

    	char *json_string = cJSON_Print(root);
    	free(json_data.value);
    	cJSON_Delete(root);
	
	// Send the value to the queue
	
	xStatus = xQueueSendToBack( xQueue, json_string, 0 );

        if( xStatus != pdPASS )
        {
            /* The send operation could not complete because the queue was full-
               this must be an error as the queue should never contain more than
               one item! */
            ESP_LOGI(TAG, "Could not send to the queue.\r\n" );
        }
	
     }

}

esp_err_t data_init(void)
{

//create task on cpu1 get_measurment
    xTaskCreate( get_measurement, "GetMeasurement", 1000, NULL, 1, NULL );
    xQueue = xQueueCreate( 5, sizeof( json_data_t ) );

    return xQueue == NULL ? ESP_FAIL : ESP_OK;
}
