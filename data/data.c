#include <string.h>

#include <freertos/FreeRTOS.h>
#include <esp_http_server.h>
#include <freertos/task.h>
#include <esp_ota_ops.h>
#include "esp_log.h"

#include "main.h"
#include "cJSON.h"
#include "data.h"

static const char* TAG = "data_mgt";

QueueHandle_t xQueue;

void set_json_data (cJSON *root, json_data_t *json_data)
{

    cJSON_AddNumberToObject(root, "t", json_data->tag);
    cJSON_AddNumberToObject(root, "l", json_data->length);
    cJSON_AddStringToObject(root, "v", json_data->value);

}

void get_measurement(void *pvParameters)
{

    json_data_t json_data; 
    const TickType_t xDelay = 500 / portTICK_PERIOD_MS; //pdMS_TO_TICKS( 500ms );
    TickType_t xLastWakeTime=xTaskGetTickCount();

    while (1) {

	    memset(json_data.value, 0, 32*sizeof(char));

	    //here we should add a switch case, base on a queue intertask data
	    //in wich, we choose wich data should be updated
	    get_chip_info_model(json_data.value);
	    json_data.length=strlen(json_data.value);

	    if (json_data.length !=0) {

		json_data.tag = CHIP_INFO_MODEL_DATA_TAG;//should be the same value as in the switch case

		cJSON *root = cJSON_CreateObject();

		set_json_data(root,&json_data);

		char *json_string_send = cJSON_Print(root);
		cJSON_Delete(root);
		
		// Send the value to the queue
		//ESP_LOGI(TAG, "get_measurement place measures in queue: \n%s\n", json_string_send);
		xQueueSend( xQueue, json_string_send, 0 );

	    }

	    get_chip_info_revision(json_data.value);
            json_data.length=strlen(json_data.value);
	    
	    if (json_data.length !=0) {

		json_data.tag = CHIP_INFO_REVISION_DATA_TAG;//should be the same value as in the switch case

		cJSON *root = cJSON_CreateObject();

		set_json_data(root,&json_data);

		char *json_string_send = cJSON_Print(root);
		cJSON_Delete(root);
		
		// Send the value to the queue
		//ESP_LOGI(TAG, "get_measurement place measures in queue: \n%s\n", json_string_send);
		xQueueSend( xQueue, json_string_send, 0 );

	    }

            vTaskDelayUntil( &xLastWakeTime, xDelay);

     }

//TODO add a counter to send in the queue (in same issue)

}

#if 0
TaskHandle_t ReceiverTaskHandle = NULL;
void ReceiverDebugTask(void *arg)
{
  char *rxbuff;

    while(1){ 
 
      rxbuff = malloc( 1024*sizeof(char));
 
      if(xQueueReceive(xQueue, rxbuff , (TickType_t)5 )) 
        printf("ReceiverTask: (%p) got a data from queue - \n%s\n",xQueue, rxbuff);
      else
	printf("ReceiverTask: (%p) no data in queue found\n", xQueue);
    
      free(rxbuff);
      vTaskDelay(pdMS_TO_TICKS(1000));
   }

}
#endif

esp_err_t data_init(void)
{
    //create the Queue communication beetwen task0 (http server) and task1 (data fetch management )
    //5 is the number of element in the queue
    //so it is the number of measures (type json_data) that can handled in a time (in 500 ms sampling of get_measurement) 
    // + 1 char for {
    // + 3 char for \n\t\" + 1 char for "t" + 5 char for \":\t<tag_value>,
    // + 3 char for \n\t\" + 1 char for "l" + 5 char for \":\t<length_value>,
    // + 3 char for \n\t\" + 1 char for "v" + (4+32+2) char for \":\t\"<string_value[32]>\"\n
    // + 1 char for }
    xQueue = xQueueCreate( 5, (1 + 9 + 9 + 42 + 1) * sizeof( char ) ); 
    if (xQueue == NULL) {
        printf("Failed to create xQueue\n");
        return ESP_FAIL;
    }

    //xTaskCreate(ReceiverDebugTask, "ReceiverDebugTask", 4096, NULL, 10, &ReceiverTaskHandle);
 
    //create task1 on cpu0 get_measurment
    xTaskCreate( get_measurement, "GetMeasurement", 4096, NULL, tskGET_MEASURMENT, NULL );

    return xQueue == NULL ? ESP_FAIL : ESP_OK;
}
