
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "data.h"


TaskHandle_t myTask3Handle = NULL;
TaskHandle_t myTask4Handle = NULL;
QueueHandle_t queue2;
QueueHandle_t xQueue;

 void task3(void *arg)
{

char txbuff[50];

queue2= xQueueCreate(5, sizeof(txbuff));

 if( queue2 == 0 )
 {
    printf("failed to create queue2= %p \n",queue2); // Failed to create the queue.

 }


sprintf(txbuff,"hello world! 1");
xQueueSend(queue2, (void*)txbuff , (TickType_t)0 );


sprintf(txbuff,"hello world! 2");
xQueueSend(queue2, (void*)txbuff , (TickType_t)0 );


sprintf(txbuff,"hello world! 3");
xQueueSendToFront(queue2, (void*)txbuff , (TickType_t)0 );

 while(1){  

   printf("FDEC data waiting to be read : %d  available spaces: %d \n",uxQueueMessagesWaiting(queue2),uxQueueSpacesAvailable(queue2));

   vTaskDelay(pdMS_TO_TICKS(1000)); }
}

 void task4(void *arg)
{
char rxbuff[50];
while(1)

  { 
//if(xQueuePeek(queue2, &(rxbuff) , (TickType_t)5 ))
if(xQueueReceive(queue2, &(rxbuff) , (TickType_t)5 ))
{ 
printf("got a data from queue!  ===  %s \n",rxbuff); }




   vTaskDelay(pdMS_TO_TICKS(1000));
   }
}

esp_err_t data_init()
{

    xTaskCreate(task3, "task3", 4096, NULL, 10, &myTask3Handle);
    xTaskCreate(task4, "task4", 4096, NULL, 10, &myTask4Handle);
 //   xTaskCreatePinnedToCore(task4, "task4", 4096, NULL, 10, &myTask2Handle,1);
 //
    return ESP_OK;
}
