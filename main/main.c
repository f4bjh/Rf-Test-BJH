
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include <esp_ota_ops.h>
#include <nvs_flash.h>


#include "data.h"

#if 0
TaskHandle_t myTask1Handle = NULL;
TaskHandle_t myTask2Handle = NULL;
QueueHandle_t queue1;

 void task1(void *arg)
{

char txbuff[50];

queue1= xQueueCreate(5, sizeof(txbuff));

 if( queue1 == 0 )
 {
    printf("failed to create queue1= %p \n",queue1); // Failed to create the queue.

 }


sprintf(txbuff,"hello world! 1");
xQueueSend(queue1, (void*)txbuff , (TickType_t)0 );


sprintf(txbuff,"hello world! 2");
xQueueSend(queue1, (void*)txbuff , (TickType_t)0 );


sprintf(txbuff,"hello world! 3");
xQueueSendToFront(queue1, (void*)txbuff , (TickType_t)0 );

 while(1){  

   printf("data waiting to be read : %d  available spaces: %d \n",uxQueueMessagesWaiting(queue1),uxQueueSpacesAvailable(queue1));

   vTaskDelay(pdMS_TO_TICKS(1000)); }
}

 void task2(void *arg)
{
char rxbuff[50];
while(1)

  { 
//if(xQueuePeek(queue1, &(rxbuff) , (TickType_t)5 ))
if(xQueueReceive(queue1, &(rxbuff) , (TickType_t)5 ))
{ 
printf("got a data from queue!  ===  %s \n",rxbuff); }




   vTaskDelay(pdMS_TO_TICKS(1000));
   }
}
#endif

void app_main()
{
	esp_err_t ret = nvs_flash_init();

	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}

	ESP_ERROR_CHECK(data_init()); 

	/* Mark current app as valid */
	const esp_partition_t *partition = esp_ota_get_running_partition();
	printf("Currently running partition: %s\r\n", partition->label);

	esp_ota_img_states_t ota_state;
	if (esp_ota_get_state_partition(partition, &ota_state) == ESP_OK) {
		if (ota_state == ESP_OTA_IMG_PENDING_VERIFY) {
			esp_ota_mark_app_valid_cancel_rollback();
		}
	}

#if 0
    xTaskCreate(task1, "task1", 4096, NULL, 10, &myTask1Handle);
    xTaskCreate(task2, "task2", 4096, NULL, 10, &myTask2Handle);
 //   xTaskCreatePinnedToCore(task2, "task2", 4096, NULL, 10, &myTask2Handle,1);

#endif

    while(1) vTaskDelay(10);
}
