#include "meas.h"

static char TAG[] = "rf_gen";

extern uint32_t ADF4351_steps[7]; 

void rf_gen_task(void *arg) 
{
   
  rf_gen_task_arg_t *rf_gen_task_arg=arg;
  meas_t *measure = rf_gen_task_arg->measure;
  ADF4351_cfg_t vfo = rf_gen_task_arg->vfo;
 
  measure->ready=false;

    while(1)
    {
        vTaskDelay(2500 / portTICK_PERIOD_MS);
        ADF4351_disable(&vfo);
        vTaskDelay(2500 / portTICK_PERIOD_MS);
        ADF4351_enable(&vfo);
    }
}

esp_err_t init_rf_gen(meas_t *measure)
{
   static rf_gen_task_arg_t rf_gen_task_arg;
  
   measure->size = sizeof(uint8_t);
   measure->pdata = malloc(measure->size * sizeof(uint8_t));
   measure->pdata_cache = malloc(measure->size * sizeof(uint8_t));
   measure->meas_func = NULL;
   //TODO add an update_func function
   //wich, based on measure->meas_param_in will update RF generator output
   //measure->update_func = update_func;
   
   rf_gen_task_arg.measure=measure;


   //ADF4351 device configuration struct
    static ADF4351_cfg_t vfo = 
    {
        .pwrlevel = 3,
        .RD2refdouble = 0, ///< ref doubler off
        .RD1Rdiv2 = 0,   ///< ref divider off
        .ClkDiv = 150,
        .BandSelClock = 200,
        .RCounter = 1,  ///< R counter to 1 (no division)
        .ChanStep = 1000000,  ///< set to 1 MHz steps
        .pins = 
        {
            .gpio_ce = GPIO_RF_GEN_CE,
            .gpio_cs = 14, // dummy pin
            .gpio_le = GPIO_RF_GEN_LE, 
            .gpio_sclk = GPIO_RF_GEN_CLK,
            .gpio_mosi = GPIO_RF_GEN_DATA, 
            .gpio_miso = 11, // dummy pin
            .gpio_ld = GPIO_RF_GEN_PLL_LOCKED,
        }
    };

    ESP_LOGI(TAG, "Initialising ADF4351...");
    ADF4351_initialise(&vfo); // initialise the chip
    ESP_LOGI(TAG, "ADF initialisation finished!");

    if(ADF4351_set_ref_freq(&vfo, 25000000) != 0)
        ESP_LOGE(TAG, "Reference frequency input invalid");

    ADF4351_enable(&vfo); // power on the device

    ADF4351_set_freq(&vfo, 440000000); // set output frequency to 440MHz
    ESP_LOGI(TAG, "Frequency set to 440Mhz");

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

  //TODO : disable ADF4531 and so on...

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

