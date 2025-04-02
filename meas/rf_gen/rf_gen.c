#include "meas.h"

static char TAG[] = "rf_gen";

extern uint32_t ADF4351_steps[7]; 

//ADF4351 device configuration struct
static ADF4351_cfg_t vfo ={
  .pwrlevel = 3,
  .RD2refdouble = 0, ///< ref doubler off
  .RD1Rdiv2 = 0,   ///< ref divider off
  .ClkDiv = 150,
  .BandSelClock = 200,
  .RCounter = 1,  ///< R counter to 1 (no division)
  .ChanStep = 1000000,  ///< set to 1 MHz steps
  ._spi_initialised = false,
  .pins = {
    .gpio_ce = GPIO_RF_GEN_CE,
    .gpio_cs = 14, // dummy pin
    .gpio_le = GPIO_RF_GEN_LE, 
    .gpio_sclk = GPIO_RF_GEN_CLK,
    .gpio_mosi = GPIO_RF_GEN_DATA, 
    .gpio_miso = -1, // dummy pin
    .gpio_ld = GPIO_RF_GEN_PLL_LOCKED,
  }
};

void rf_gen_task(void *arg) 
{
   
  rf_gen_task_arg_t *rf_gen_task_arg=arg;
  meas_t *measure = rf_gen_task_arg->measure;
  //ADF4351_cfg_t vfo = rf_gen_task_arg->vfo;
 
  measure->ready=false;

  while(1) {
       vTaskDelay(100 / portTICK_PERIOD_MS);

       //TO DO
       //
       // check if lock detect is on

  }
}

//TODO
esp_err_t update_rf_gen(meas_t *measure)
{

  int err;
  int freq;

  freq = measure->meas_param_in[1];
  freq |=  (measure->meas_param_in[2]&0xFF)<<8;
  freq |=  (measure->meas_param_in[3]&0xFF)<<16;
  freq |=  (measure->meas_param_in[4]&0xFF)<<24;

  //
  //set output frequency
  //
  ESP_LOGI(TAG, "update frequency to %dHz",freq);
  err=ADF4351_set_freq(&vfo, freq); // set output frequency to 440MHz
  if (err!=0)
     ESP_LOGE(TAG,"failed to update output frequency");


  //
  // set power level (TO DO)
  // measure->meas_param_in[5] direct in register

  //
  // enable/disable device
  //
  ESP_LOGI(TAG, "%s ADF5351 device",measure->meas_param_in[0] == 1 ? "enable": "disable");
  if ( measure->meas_param_in[0] == 1)
    ADF4351_enable(&vfo); // power on the device
  else
    ADF4351_disable(&vfo);


  return ESP_OK;
}

esp_err_t init_rf_gen(meas_t *measure)
{
   static rf_gen_task_arg_t rf_gen_task_arg;
   int err;
  
   measure->size = sizeof(uint8_t);
   measure->pdata = malloc(measure->size * sizeof(uint8_t));
   measure->pdata_cache = malloc(measure->size * sizeof(uint8_t));
   measure->meas_func = NULL;
   int freq;

   freq = measure->meas_param_in[1];
   freq |=  (measure->meas_param_in[2]&0xFF)<<8;
   freq |=  (measure->meas_param_in[3]&0xFF)<<16;
   freq |=  (measure->meas_param_in[4]&0xFF)<<24;

   rf_gen_task_arg.measure=measure;


    if (!(vfo._spi_initialised )) {
      ESP_LOGI(TAG, "Initialising ADF4351...");
      ADF4351_initialise(&vfo); // initialise the chip
      ESP_LOGI(TAG, "ADF initialisation finished!");
    }

    if(ADF4351_set_ref_freq(&vfo, 25000000) != 0)
        ESP_LOGE(TAG, "Reference frequency input invalid");

    //ADF4351_enable(&vfo); // power on the device

   ESP_LOGI(TAG, "init frequency set to %dHz",freq);
   err = ADF4351_set_freq(&vfo, freq);
    if (err!=0)
      ESP_LOGE(TAG,"failed to update output frequency");


    ESP_LOGI(TAG, "init frequency set to %dHz",freq);

   rf_gen_task_arg.vfo = vfo;

    //create a rf_gen on CPU1
   //CPU number set in measure->meas_param_in[0]
   xTaskCreatePinnedToCore(rf_gen_task, 
		   "rf_gen task", 
		   configMINIMAL_STACK_SIZE, 
		   &rf_gen_task_arg,
		   tskIDLE_PRIORITY + 1,
		   (TaskHandle_t*) &(measure->handle),
		   1);

   return ESP_OK;
}

esp_err_t stop_rf_gen(meas_t *measure)
{
  static TaskHandle_t task_handle;

  task_handle = measure->handle;

  if (task_handle!=NULL)
    vTaskDelete(task_handle);

  ADF4351_disable(&vfo);

  return ESP_OK;
}

esp_err_t calc_rf_gen(instance_meas_t *instance_meas)
{
	//TO DO
#if 0
    meas_t measure=instance_meas->measures;
    memset(instance_meas->calc_value, 0, CALC_VALUE_SIZE*sizeof(char));
 
    sprintf(instance_meas->calc_value,"%d", *(measure.pdata_cache)/10 );
#endif
   
    return ESP_OK;
}

