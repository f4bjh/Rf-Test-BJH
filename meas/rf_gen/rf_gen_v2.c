#include <freertos/FreeRTOS.h>
#include <esp_http_server.h>
#include <freertos/task.h>
#include <esp_ota_ops.h>
#include "esp_log.h"
#include <sys/param.h>

#include "meas_mgt.h"
#include "meas.h"
#include "gpio.h"

static char TAG[] = "rf_gen";

extern uint32_t adf4351_steps[7]; 

//adf4351 device configuration struct
static adf4351_cfg_t vfo ={
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
  },
  .adf4351_device = NULL
};

adf4351_init_param default_adf4351_init_param = {
		/* Device settings */
		.clkin = 25000000,
		.channel_spacing = 10000,
		.power_up_frequency = 2500000000ul,
		.reference_div_factor = 0,
		.reference_doubler_enable = 0,
		.reference_div2_enable = 0,

		/* r2_user_settings */
		.phase_detector_polarity_positive_enable = 1,
		.lock_detect_precision_6ns_enable = 0,
		.lock_detect_function_integer_n_enable = 0,
		.charge_pump_current = 2500,
		.muxout_select = 0,
		.low_spur_mode_enable = 0,

		/* r3_user_settings */
		.cycle_slip_reduction_enable = 0,
		.charge_cancellation_enable = 0,
		.anti_backlash_3ns_enable = 0,
		.band_select_clock_mode_high_enable = 0,
		.clk_divider_12bit = 0,
		.clk_divider_mode = 0,

		/* r4_user_settings */
		.aux_output_enable = 0,
		.aux_output_fundamental_enable = 1,
		.mute_till_lock_enable = 0,
		.output_power = 3,
		.aux_output_power = 0,
	};


void rf_gen_task(void *arg) 
{
   
  rf_gen_task_arg_t *rf_gen_task_arg=arg;
  meas_t *measure = rf_gen_task_arg->measure;
  //adf4351_cfg_t vfo = rf_gen_task_arg->vfo;
 
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

  int freq;
  int64_t calc_freq;

  freq = measure->meas_param_in[1];
  freq |=  (measure->meas_param_in[2]&0xFF)<<8;
  freq |=  (measure->meas_param_in[3]&0xFF)<<16;
  freq |=  (measure->meas_param_in[4]&0xFF)<<24;

  //
  //set output frequency
  //
  ESP_LOGI(TAG, "update frequency to %dHz",freq);
  calc_freq=adf4351_set_freq(vfo.adf4351_device, freq); // set output frequency to 440MHz
  ESP_LOGI(TAG, "set frequency to %lluHz",calc_freq);
  

  //
  // set power level (TO DO)
  // measure->meas_param_in[5] direct in register

  //
  // enable/disable device
  //
  ESP_LOGI(TAG, "%s ADF5351 device",measure->meas_param_in[0] == 1 ? "enable": "disable");
  if ( measure->meas_param_in[0] == 1)
    adf4351_enable(&vfo); // power on the device
  else
    adf4351_disable(&vfo);


  return ESP_OK;
}

esp_err_t init_rf_gen(meas_t *measure)
{
   static rf_gen_task_arg_t rf_gen_task_arg;
   int err;
   static adf4351_dev *adf4351_device; //TO DO : move vfo into this one
	
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
      ESP_LOGI(TAG, "Initialising bus...");
      adf4351_initialise(&vfo); // initialise the chip
    }

    ESP_LOGI(TAG, "Initialising ADF4351...");
    err = adf4351_setup(&adf4351_device, default_adf4351_init_param);
    if (err != 0) {
	ESP_LOGE(TAG,"adf4351_setup() failed!");
	return ESP_FAIL;
    }
    if (adf4351_device == NULL) {
	ESP_LOGE(TAG,"adf4351 not found");
	return ESP_ERR_NOT_FOUND;
    }

    memcpy(&(adf4351_device->pins),&vfo.pins,sizeof(pin_settings));

    adf4351_out_altvoltage0_refin_frequency(adf4351_device, adf4351_device->pdata->clkin);
    adf4351_out_altvoltage0_frequency_resolution(adf4351_device,adf4351_device->pdata->channel_spacing);
    adf4351_out_altvoltage0_frequency(adf4351_device, adf4351_device->pdata->power_up_frequency);



    vfo.adf4351_device = adf4351_device;

#if 0
    if(adf4351_set_ref_freq(&vfo, 25000000) != 0)
        ESP_LOGE(TAG, "Reference frequency input invalid");

    //adf4351_enable(&vfo); // power on the device

   ESP_LOGI(TAG, "init frequency set to %dHz",freq);
   err = adf4351_set_freq(&vfo, freq);
    if (err!=0)
      ESP_LOGE(TAG,"failed to update output frequency");


    ESP_LOGI(TAG, "init frequency set to %dHz",freq);
#endif

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

  adf4351_disable(&vfo);
  adf4351_remove(&vfo);

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

