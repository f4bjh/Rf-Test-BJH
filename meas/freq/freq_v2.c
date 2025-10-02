#include <freertos/FreeRTOS.h>
#include <esp_http_server.h>
#include <freertos/task.h>
#include <esp_ota_ops.h>
#include "esp_log.h"

#include "meas_mgt.h"
#include "meas.h"

static char TAG[] = "frequencymeter";

static reciproc_freq_cfg_t fpga_freq={
  .pins = {
    .gpio_ce = GPIO_FREQUENCYMETER_CE,
    .gpio_cs = 14, // dummy pin
    .gpio_le = 14, //dummy pin
    .gpio_ld = 14, //dummy pin
  },
  ._spi_initialised = false,
  ._ce_initialised = false,
  ._enabled = false,
  .reciproc_freq_device=NULL
};

reciproc_freq_init_param default_reciproc_freq_init_param = {
	.window_time_ms = 1000,
	.ref_fast_clk_MHz = 100,
	.square_out_freq = 10000000 //10MHz

};

void frequencymeter_task(void *arg) 
{
   
  frequencymeter_task_arg_t *frequencymeter_task_arg=arg;
  meas_t *measure = frequencymeter_task_arg->measure;
  //reciproc_freq_cfg_t fpga_freq;
  ESP_LOGI(TAG,"Starting frequencymeter_task");

  
  //memcpy(&(fpga_freq), &(frequencymeter_task_arg->fpga_freq), sizeof(reciproc_freq_cfg_t));
 
  measure->ready=false;

  ESP_LOGI(TAG,"Starting frequencymeter_task 2");

  while(1) {
       vTaskDelay(10 / portTICK_PERIOD_MS);

       ESP_LOGI(TAG,"Talking to FPGA over SPI...");	
       reciproc_freq_TEST_WRITE_SPI_FPGA(&fpga_freq);
      
  }
}

esp_err_t init_frequencymeter(meas_t *measure)
{
   int err;
   static reciproc_freq_dev *reciproc_freq_device;
   frequencymeter_task_arg_t *frequencymeter_task_arg=malloc(sizeof(frequencymeter_task_arg_t));; 
 
   measure->size = sizeof(int);
   measure->pdata = malloc(measure->size * sizeof(uint8_t));
   measure->pdata_cache = malloc(measure->size * sizeof(uint8_t));
   measure->meas_func = NULL;

   frequencymeter_task_arg->measure=measure;


   if (!(fpga_freq._spi_initialised )) {
      ESP_LOGI(TAG, "Initialising bus...");
      reciproc_freq_initialise(&fpga_freq); // initialise the chip
   }

   ESP_LOGI(TAG, "Initialising FPGA reciprocal frequency meas...");
   err = reciproc_freq_setup(&reciproc_freq_device, default_reciproc_freq_init_param);
   if (err != 0) {
	ESP_LOGE(TAG,"reciproc_freq_setup() failed!");
	return ESP_FAIL;
   }
   if (reciproc_freq_device == NULL) {
	ESP_LOGE(TAG,"reciproc_freq not found");
	return ESP_ERR_NOT_FOUND;
    }

    ESP_LOGI(TAG,"F4BJH 1");
    memcpy(&(reciproc_freq_device->pins),&fpga_freq.pins,sizeof(pin_settings));
    ESP_LOGI(TAG,"F4BJH 2");
 
    fpga_freq.reciproc_freq_device = reciproc_freq_device;
    ESP_LOGI(TAG,"F4BJH 3");
 
    frequencymeter_task_arg->fpga_freq = fpga_freq;
    ESP_LOGI(TAG,"F4BJH 4");
 
    //create a frequencymeter on CPU1
    if ( xTaskCreatePinnedToCore(frequencymeter_task, 
		   "frequencymeter task", 
		   4096, 
		   frequencymeter_task_arg,
		   tskIDLE_PRIORITY + 1,
		   (TaskHandle_t*) &(measure->handle),
		   1) == pdPASS)    {
	    vTaskSetThreadLocalStoragePointer(measure->handle, 0, frequencymeter_task_arg);
    };

    ESP_LOGI(TAG,"F4BJH 5");
 
   return ESP_OK;

}

esp_err_t stop_frequencymeter(meas_t *measure)
{
  static TaskHandle_t task_handle;

  task_handle = measure->handle;

  if (task_handle!=NULL) {
    vTaskDelete(task_handle);


    // libérer la mémoire passée à la tâche
    frequencymeter_task_arg_t *frequencymeter_task_arg =
        (frequencymeter_task_arg_t *) pvTaskGetThreadLocalStoragePointer(measure->handle, 0);

    if (frequencymeter_task_arg) {
        free(frequencymeter_task_arg);
    }

    measure->handle = NULL;

  }
  return ESP_OK;
}

esp_err_t calc_frequencymeter(instance_meas_t *instance_meas)
{
    meas_t measure=instance_meas->measures;
    memset(instance_meas->calc_value, 0, CALC_VALUE_SIZE*sizeof(char));
    unsigned int freq=0;

    //au depart,l'objectif etait d'obtenir une precision de 1mHz, sur le calibre 10MHz
    //mais ca suppose des lors d'avoir une fene^tre de 1000*1s...soit 1000s...ce qui n'est pas raisonnable
    //alors, a moins d'avoir correctement resolut li'ssue n°79 sur gitlab, qui vise a obtenir et ameliorer
    //la precision pour obtenir cette precision attendue de 1mHz, pour le moment, on met en place un
    //espece de contournement, dans la but d'affichier artificiellement une frequence avec une precision de 1mHz
    //par exemple, pour 4MHz, on va afficher 4 000 000, 000 Hz. Les 3 zero aprs la virgule etant ajouté hardcodé ci-dessous
    //note : on est oblige de passer un variable tampon "freq", car pdata_cache est un pointeur sur un octet seulement


    freq = *(measure.pdata_cache);
    freq |= ((*(measure.pdata_cache+1))<<8)&0xFF00;
    freq |= ((*(measure.pdata_cache+2))<<16)&0xFF0000;
    freq |= ((*(measure.pdata_cache+3))<<24)&0xFF000000;

    sprintf(instance_meas->calc_value,"%llu", (unsigned long long int) 4*freq*1000 );

    return ESP_OK;
}



