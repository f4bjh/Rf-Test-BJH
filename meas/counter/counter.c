#include "meas.h"

//standalone task on cpu1
//should fill instance_meas->measures.pdata
void counter_task(void *arg)
{
  uint8_t counter=1;
  counter_task_arg_t *counter_task_arg=arg;
  meas_t *measure = counter_task_arg->measure;
 

  while(1) {   
     
        vTaskDelay(100 / portTICK_PERIOD_MS);
	
	if (counter == 100)
	  counter=1;
	else
          counter++;
	if ( (counter % 10)==0) {
	  *(measure->pdata) = counter;
	  measure->ready=true;
	}
  };
}

esp_err_t init_counter(meas_t *measure)
{
   static counter_task_arg_t counter_task_arg;
     
   measure->size = sizeof(uint8_t);
   measure->pdata = malloc(measure->size * sizeof(uint8_t));
   measure->pdata_cache = malloc(measure->size * sizeof(uint8_t));
   measure->meas_func = NULL;
   
   counter_task_arg.measure=measure;

   //create a counter task on a specific CPU 0 or 1
   //CPU number set in measure->meas_param_in[0]
   xTaskCreatePinnedToCore(counter_task, 
		   "counter task", 
		   configMINIMAL_STACK_SIZE, 
		   &counter_task_arg,
		   tskIDLE_PRIORITY + 1,
		   (TaskHandle_t*) &(measure->handle),
		   measure->meas_param_in[0]);

   return ESP_OK;

}

esp_err_t stop_counter(meas_t *measure)
{
  static TaskHandle_t task_handle;

  task_handle = measure->handle;

  if (task_handle!=NULL)
    vTaskDelete(task_handle);

  return ESP_OK;
}


esp_err_t calc_counter(instance_meas_t *instance_meas)
{
    meas_t measure=instance_meas->measures;
    memset(instance_meas->calc_value, 0, CALC_VALUE_SIZE*sizeof(char));
 
    sprintf(instance_meas->calc_value,"%d", *(measure.pdata_cache)/10 );
   
    return ESP_OK;
}



