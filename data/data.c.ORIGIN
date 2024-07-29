#include <string.h>

#include <freertos/FreeRTOS.h>
#include <esp_http_server.h>
#include <freertos/task.h>
#include <esp_ota_ops.h>
#include "esp_log.h"

#include "main.h"
#include "cJSON.h"
#include "data.h"

QueueHandle_t xQueue;

void get_measurement(void *pvParameters)
{

    json_data_t json_data; 
    const TickType_t xDelay = 500 / portTICK_PERIOD_MS;
    TickType_t xLastWakeTime=xTaskGetTickCount();

    while (1) {

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
		
		xQueueSendToBack( xQueue, json_string, 0 );

	    }

            vTaskDelayUntil( &xLastWakeTime, xDelay);

     }
}

esp_err_t data_init(void)
{
    //create the Queue communication beetwen task0 (http server) and task1 (data fetch management )
    //5 is the number of element in the queue
    //so it is the number of measures that can handled in a time (in 500 ms sampling of get_measurement)
     xQueue = xQueueCreate( 5, sizeof( json_data_t ) );

    //create task1 on cpu0 get_measurment
    xTaskCreate( get_measurement, "GetMeasurement", 1000, NULL, tskGET_MEASURMENT, NULL );

    return xQueue == NULL ? ESP_FAIL : ESP_OK;
}
