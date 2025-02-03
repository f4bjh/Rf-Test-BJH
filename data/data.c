#include <string.h>

#include <freertos/FreeRTOS.h>
#include <esp_http_server.h>
#include <freertos/task.h>
#include <esp_ota_ops.h>
#include "esp_log.h"

#include "main.h"
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
    const TickType_t xDelay = HTTP_SERVER_WAKE_UP_TICK / portTICK_PERIOD_MS;
    //TickType_t xLastWakeTime=xTaskGetTickCount();
    uint8_t retries = 0;
    int counter=0;
    T_DATA_TO_SEND data_to_send = DATA_TO_SEND_IS_CHIP_NAME;
    char *json_string_send=NULL;
    cJSON *root;

    while (1) {
	    
	    retries = 0;

	    memset(json_data.value, 0, 32*sizeof(char));

	    switch(data_to_send) {
 	      case DATA_TO_SEND_IS_CHIP_NAME:
		ESP_LOGV("TAG", "get_measurement: get DATA_TO_SEND_IS_CHIP_NAME(%d)",data_to_send);
		get_chip_info_model(json_data.value);
  	        json_data.length=strlen(json_data.value);

	        if (json_data.length !=0) {

		  json_data.tag = CHIP_INFO_MODEL_DATA_TAG;//should be the same value as in the switch case
		  root = cJSON_CreateObject();
		  
		  set_json_data(root,&json_data);

		  json_string_send = cJSON_Print(root);
		  cJSON_Delete(root);
		}
	        break;
	      case DATA_TO_SEND_IS_CHIP_VERSION:
		ESP_LOGV("TAG", "get_measurement: get DATA_TO_SEND_IS_CHIP_VERSION(%d)",data_to_send);
	        get_chip_info_revision(json_data.value);
                json_data.length=strlen(json_data.value);
	    
	        if (json_data.length !=0) {

		  json_data.tag = CHIP_INFO_REVISION_DATA_TAG;//should be the same value as in the switch case
		  root = cJSON_CreateObject();

		  set_json_data(root,&json_data);

		  json_string_send = cJSON_Print(root);
		  cJSON_Delete(root);
		}
	        break;
	      case DATA_TO_SEND_IS_CURRENT_PARTITION:
		ESP_LOGV(TAG,"get_measurment : get DATA_TO_SEND_IS_CURRENT_PARTITION(%d)",data_to_send);
		const esp_partition_t *partition = esp_ota_get_running_partition();
		sprintf(json_data.value,"%s", partition->label);
		json_data.length=strlen(json_data.value);
		ESP_LOGI(TAG, "Currently running partition: %s", json_data.value);
		if (json_data.length !=0) {

		  json_data.tag = CURRENT_PARTITION_NAME_TAG;//should be the same value as in the switch case
		  root = cJSON_CreateObject();

		  set_json_data(root,&json_data);

		  json_string_send = cJSON_Print(root);
		  cJSON_Delete(root);
		}
		break;
	      case DATA_TO_SEND_IS_COUNTER:
	      default:
		ESP_LOGV("TAG", "get_measurement: get DATA_TO_SEND_IS_COUNTER(%d)",data_to_send);
		sprintf(json_data.value,"%d", counter);
		json_data.length=strlen(json_data.value);
	        if (json_data.length !=0) {

		  json_data.tag = COUNTER_VALUE_TAG;//should be the same value as in the switch case
		  root = cJSON_CreateObject();

		  set_json_data(root,&json_data);

		  json_string_send = cJSON_Print(root);
		  cJSON_Delete(root);
		}
		counter++;
		break;
            }

  	   // Send the value to the queue
	   ESP_LOGI(TAG, "get_measurement: place measures(%d) in queue (%p)", data_to_send, xQueue);
	   ESP_LOGV(TAG, "%s",json_string_send);
	   //xQueueSend( xQueue, json_string_send, 0 );
	   while( (xQueueSend( xQueue, json_string_send, xDelay ) == errQUEUE_FULL) && (retries <5)) {
	     retries++;
	   }

	   if (retries == 5) 
  	     ESP_LOGI(TAG, "get_measurement: failed to place measures(%d) in queue(%p) : retries = %d", data_to_send--, xQueue,retries);
	   else
	     ESP_LOGI(TAG, "get_measurement: succeed to place measures(%d) - retries=%d", data_to_send, retries);

	   data_to_send = data_to_send == DATA_TO_SEND_IS_COUNTER ? DATA_TO_SEND_IS_COUNTER : data_to_send+1;


	    //}
            //vTaskDelayUntil( &xLastWakeTime, xDelay);

     }

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
    xQueue = xQueueCreate( NB_OF_MEASUREMENT_IN_QUEUE, JSON_STRING_SIZE_OF_ONE_MEASUREMENT * sizeof( char ) ); 
    if (xQueue == NULL) {
        printf("Failed to create xQueue\n");
        return ESP_FAIL;
    }

    //xTaskCreate(ReceiverDebugTask, "ReceiverDebugTask", 4096, NULL, 10, &ReceiverTaskHandle);
 
    //create task1 on cpu0 get_measurment
    xTaskCreate( get_measurement, "GetMeasurement", 4096, NULL, tskGET_MEASURMENT, NULL );

    return xQueue == NULL ? ESP_FAIL : ESP_OK;
}
