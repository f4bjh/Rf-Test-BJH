#include <inttypes.h>
#include "main.h"
#include "reciproc_freq_meas.h"
#include "freq.h"

static char TAG[] = "freq";

static reciproc_freq_cfg_t fpga_freq={
  .pins = {
    .gpio_ce = {
      .gpio_number = GPIO_FREQUENCYMETER_CS_N,
      .polarity = ACTIVE_LOW
     },
    .gpio_le = {
      .gpio_number = GPIO_DUMMY
     },
    .gpio_ld = {
      .gpio_number = GPIO_DUMMY
     },
    .gpio_por = {
      .gpio_number = GPIO_SPI_POR,
      .polarity = ACTIVE_LOW
     }
  },
  ._spi_initialised = false,
  ._ce_initialised = false,
  ._por_initialised = false,
  ._enabled = false,
  .reciproc_freq_device=NULL
};

reciproc_freq_init_param default_reciproc_freq_init_param = {
	.window_time_ms = 1000,
	.ref_fast_clk_MHz = 100,
	.square_out_freq = 10000000 //10MHz

};

void read_status_task(void *arg)
{
  esp_err_t err;
  read_status_task_arg_t *read_status_task_arg=arg;
  meas_t *measure = read_status_task_arg->measure;
  //reciproc_freq_cfg_t *fpga_freq=read_status_task_arg->fpga_freq;
  uint8_t reciproc_freq_status[RECIPROC_FREQ_MEAS_RX_BYTE_SIZE_32b_word];

   ESP_LOGI(TAG,"Starting read_status_task");

   measure->ready=false;

  while(1) {
	vTaskDelay(MEASURMENT_TASK_WAKE_UP_TICK / portTICK_PERIOD_MS);

	if (fpga_freq.reciproc_freq_device) {

		if (fpga_freq.reciproc_freq_device->dev_initialised ) {

			err = reciproc_freq_read_status(&fpga_freq,reciproc_freq_status);
			  
			if (err != ESP_OK) {
				ESP_LOGE(TAG,"reciproc_freq_read_status() failed!");
			}
			   
			ESP_LOGI(TAG,"reciproc_freq_read_status=0x%02X%02X%02X%02X",
					reciproc_freq_status[0],
					reciproc_freq_status[1],
					reciproc_freq_status[2],
					reciproc_freq_status[3]);
			memcpy(measure->pdata,reciproc_freq_status, measure->size );
			measure->ready=true;
		}
	} 
  }

}
 
void frequencymeter_task(void *arg) 
{
   
  frequencymeter_task_arg_t *frequencymeter_task_arg=arg;
  meas_t *measure = frequencymeter_task_arg->measure;
  //reciproc_freq_cfg_t fpga_freq;
  uint8_t rx_status[RECIPROC_FREQ_MEAS_RX_BYTE_SIZE_32b_word];
  uint8_t nb_of_meas;
  uint8_t meas[measure->size];
  uint8_t delta_tick[2*RECIPROC_FREQ_MEAS_FIFO_DEPTH*BYTE_SIZE_32b_word];
  uint8_t N_counted[RECIPROC_FREQ_MEAS_FIFO_DEPTH*BYTE_SIZE_32b_word];
  uint8_t  led_cnt=0;
  uint8_t i,j;
  int base=0;

  ESP_LOGI(TAG,"Starting frequencymeter_task");

  measure->ready=false;
  memset(meas, 0, measure->size);
 
  //set NCO_freq
  reciproc_freq_set_freq(&fpga_freq,100000);

  //set cfg_N
  reciproc_freq_set_cfg_N(&fpga_freq,RECIPROC_FREQ_MEAS_SUB_CMD_CFG_N_1000);
  //start measurment
  reciproc_freq_start_meas(&fpga_freq);

  //Polling - Measure
  while(1) {
       vTaskDelay(MEASURMENT_TASK_WAKE_UP_TICK / portTICK_PERIOD_MS);

       ESP_LOGI(TAG,"Talking to FPGA over SPI...");
       if (led_cnt == 0)       
       	reciproc_freq_toggle_led(&fpga_freq);
       led_cnt = (led_cnt + 1) % 3;
       reciproc_freq_read_status(&fpga_freq,rx_status);
       nb_of_meas = rx_status[RECIPROC_FREQ_MEAS_BYTE_POS_FIFO_COUNT];

       	if (nb_of_meas > 0 || (rx_status[RECIPROC_FREQ_MEAS_BYTE_POS_STATUS] & RECIPROC_FREQ_MEAS_BIT_POS_STATUS_FIFO_FULL)) {
		reciproc_freq_read_delta_tick(&fpga_freq, delta_tick, nb_of_meas);
		reciproc_freq_read_N_counted(&fpga_freq, N_counted, nb_of_meas);

		//  uint8_t   | 8*64b_word  | 8*32b_word 
		// nb_of_meas |  delta_tick | N_counted
		meas[0] = nb_of_meas;
		
	    for (i = 0; i < 8*nb_of_meas; i+=8) {
#if 0
          	ESP_LOGI(TAG,"delta_tick=0x%02X%02X%02X%02X%02X%02X%02X%02X",
	             delta_tick[i+0], delta_tick[i+1], delta_tick[i+2], delta_tick[i+3],
	             delta_tick[i+4], delta_tick[i+5], delta_tick[i+6], delta_tick[i+7]);
#endif

	        for (j = 0; j < 4; j++) {
             		meas[1 + i + j] = delta_tick[i + j + 4];
             		meas[1 + i + j + 4] = delta_tick[i + j];
 
          	}
	    }			
	
    	    base = 1 + 8 * nb_of_meas;
            for (i = 0; i < 4*nb_of_meas; i+=4) {
#if 0
   		   	ESP_LOGI(TAG,"N_counted=0x%02X%02X%02X%02X",
        			N_counted[i+0], N_counted[i+1],
        			N_counted[i+2], N_counted[i+3]);
#endif

    			for (j = 0; j < 4; j++) {
        			meas[base + i + j] = N_counted[i + j];
    			}
		}
		
		memcpy(measure->pdata,meas, measure->size );
		measure->ready=true;
	}
  }
}

esp_err_t init_read_status(meas_t *measure)
{
   read_status_task_arg_t *read_status_task_arg=malloc(sizeof(read_status_task_arg_t)); 

   measure->size = 4*sizeof(uint8_t);
   measure->pdata = malloc(measure->size * sizeof(uint8_t));
   measure->pdata_cache = malloc(measure->size * sizeof(uint8_t));
   measure->meas_func = NULL;

   read_status_task_arg->measure=measure;
   read_status_task_arg->fpga_freq = fpga_freq;
 
   //create a frequencymeter on CPU1
   if ( xTaskCreatePinnedToCore(
		   read_status_task, 
		   "read status task", 
		   4096, 
		   read_status_task_arg,
		   tskIDLE_PRIORITY + 1,
		   (TaskHandle_t*) &(measure->handle),
		   1)					   == pdPASS)    {
	    vTaskSetThreadLocalStoragePointer(measure->handle, 0, read_status_task_arg);
   };

   return ESP_OK;

}
esp_err_t init_frequencymeter(meas_t *measure)
{
   int err;
   static reciproc_freq_dev *reciproc_freq_device;
   frequencymeter_task_arg_t *frequencymeter_task_arg=malloc(sizeof(frequencymeter_task_arg_t)); 
   uint8_t reciproc_freq_status[RECIPROC_FREQ_MEAS_RX_BYTE_SIZE_32b_word];


   measure->size = (2*RECIPROC_FREQ_MEAS_FIFO_DEPTH*sizeof(uint32_t)) + (RECIPROC_FREQ_MEAS_FIFO_DEPTH*sizeof(uint32_t)) + sizeof(uint8_t) ;
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

    //
    // check FPGA is OK
    //
    err = reciproc_freq_read_status(&fpga_freq,reciproc_freq_status);
    if (err != ESP_OK) {
	ESP_LOGE(TAG,"reciproc_freq_read_status() failed!");
	return ESP_FAIL;
    }
 
    memcpy(&(reciproc_freq_device->pins),&fpga_freq.pins,sizeof(pin_settings));
 
    fpga_freq.reciproc_freq_device = reciproc_freq_device;
 
    frequencymeter_task_arg->fpga_freq = fpga_freq;
 
    //create a frequencymeter on CPU1
    if ( xTaskCreatePinnedToCore(
		    frequencymeter_task, 
		   "frequencymeter task", 
		   4096, 
		   frequencymeter_task_arg,
		   tskIDLE_PRIORITY + 1,
		   (TaskHandle_t*) &(measure->handle),
		   1)					   == pdPASS)    {
	    vTaskSetThreadLocalStoragePointer(measure->handle, 0, frequencymeter_task_arg);
    };
   
   fpga_freq.reciproc_freq_device->dev_initialised=true; 
   return ESP_OK;

}

esp_err_t stop_read_status(meas_t *measure)
{
  static TaskHandle_t task_handle;

  task_handle = measure->handle;

  if (task_handle!=NULL) {
    vTaskDelete(task_handle);


    // libérer la mémoire passée à la tâche
    read_status_task_arg_t *read_status_task_arg =
        (read_status_task_arg_t *) pvTaskGetThreadLocalStoragePointer(measure->handle, 0);

    if (read_status_task_arg) {
        free(read_status_task_arg);
    }

    measure->handle = NULL;

  }
  return ESP_OK;

}
esp_err_t stop_frequencymeter(meas_t *measure)
{
  static TaskHandle_t task_handle;

  stop_reciproc_freq_meas(&fpga_freq);

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
    double freq;
    double freq_moy=0;
    uint8_t nb_of_meas=0;
    int i;
    uint64_t delta_tick;
    uint32_t N_counted;

 	ESP_LOGI(TAG,"calc_frequencymeter 0x%02X",measure.pdata_cache[0]);
	nb_of_meas = measure.pdata_cache[0];


	for (i = 0; i < nb_of_meas; i++) {

	    delta_tick =
		((uint64_t)measure.pdata_cache[1 + i*8 + 0] << 56) |
		((uint64_t)measure.pdata_cache[1 + i*8 + 1] << 48) |
		((uint64_t)measure.pdata_cache[1 + i*8 + 2] << 40) |
		((uint64_t)measure.pdata_cache[1 + i*8 + 3] << 32) |
		((uint64_t)measure.pdata_cache[1 + i*8 + 4] << 24) |
		((uint64_t)measure.pdata_cache[1 + i*8 + 5] << 16) |
		((uint64_t)measure.pdata_cache[1 + i*8 + 6] << 8)  |
		((uint64_t)measure.pdata_cache[1 + i*8 + 7]);

	    N_counted =
		((uint32_t)measure.pdata_cache[1 + 8*nb_of_meas + i*4 + 0] << 24) |
		((uint32_t)measure.pdata_cache[1 + 8*nb_of_meas + i*4 + 1] << 16) |
		((uint32_t)measure.pdata_cache[1 + 8*nb_of_meas + i*4 + 2] << 8)  |
		((uint32_t)measure.pdata_cache[1 + 8*nb_of_meas + i*4 + 3]);

	    ESP_LOGI(TAG, "N_counted = 0x%08" PRIX32, N_counted);
	    ESP_LOGI(TAG, "delta_tick = 0x%016" PRIX64, delta_tick);

	    if (delta_tick == 0) {
		ESP_LOGE(TAG, "Division par zéro !");
		freq = 0;
		continue;

	    } else {

	        freq = (100000000.0 * (double)N_counted) / (double)delta_tick;

	    }

	    freq_moy += freq;
	}

	freq_moy /= nb_of_meas;

	snprintf(instance_meas->calc_value, CALC_VALUE_SIZE, "%.3f", freq_moy);
	ESP_LOGI(TAG, "freq = %s Hz", instance_meas->calc_value);



    return ESP_OK;
}

esp_err_t calc_read_status(instance_meas_t *instance_meas)
{
    meas_t measure=instance_meas->measures;
    memset(instance_meas->calc_value, 0, CALC_VALUE_SIZE*sizeof(char));
 
    sprintf(instance_meas->calc_value,"0x%02X%02X%02X%02X",measure.pdata_cache[0],measure.pdata_cache[1],measure.pdata_cache[2],measure.pdata_cache[3]);
	   
    return ESP_OK;
}
