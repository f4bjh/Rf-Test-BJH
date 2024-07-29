
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "data.h"


TaskHandle_t SenderTaskHandle = NULL;
TaskHandle_t ReceiverTaskHandle = NULL;
QueueHandle_t xQueueTest;
QueueHandle_t xQueue;

void SenderTask(void *arg)
{

  char txbuff[50];

    sprintf(txbuff,"hello world! 1");
    xQueueSend(xQueueTest, (void*)txbuff , (TickType_t)0 );

    sprintf(txbuff,"hello world! 2");
    xQueueSend(xQueueTest, (void*)txbuff , (TickType_t)0 );

    sprintf(txbuff,"hello world! 3");
    xQueueSend(xQueueTest, (void*)txbuff , (TickType_t)0 );

#if 0
    sprintf(txbuff,"hello world! 4");
    xQueueSend(xQueueTest, (void*)txbuff , (TickType_t)0 );
#endif

    while(1){  

      printf("SenderTask: data sent (to be read=%d, available=%d)\n",uxQueueMessagesWaiting(xQueueTest),uxQueueSpacesAvailable(xQueueTest));

      vTaskDelay(pdMS_TO_TICKS(100)); 
    }
}

void ReceiverTask(void *arg)
{
  char rxbuff[50];

    while(1){ 

      //if(xQueuePeek(xQueueTest, &(rxbuff) , (TickType_t)5 ))
      //

      if(xQueueReceive(xQueueTest, &(rxbuff) , (TickType_t)5 )) 
        printf("ReceiverTask: (%p) got a data from queue - %s\n",xQueueTest, rxbuff);
      else
	printf("ReceiverTask: (%p) no data in queue found\n", xQueueTest);
    
      vTaskDelay(pdMS_TO_TICKS(1000));
   }

}

esp_err_t data_init()
{
    xQueueTest = xQueueCreate(5, sizeof(char[50]));
    if (xQueueTest == NULL) {
        printf("Failed to create xQueueTest\n");
        return ESP_FAIL;
    }

    xTaskCreate(ReceiverTask, "ReceiverTask", 4096, NULL, 10, &ReceiverTaskHandle);
    xTaskCreate(SenderTask, "SenderTask", 4096, NULL, 10, &SenderTaskHandle);
 
    //xTaskCreatePinnedToCore(task4, "task4", 4096, NULL, 10, &myTask2Handle,1);

    return ESP_OK;
}
