#include <freertos/FreeRTOS.h>
#include <esp_http_server.h>
#include <freertos/task.h>
#include <esp_ota_ops.h>
#include "esp_log.h"
#include "driver/gptimer.h"
#include "driver/pulse_cnt.h"

#include "gpio.h"
#include "meas_mgt.h"
#include "meas.h"

#define PCNT_INPUT_GPIO      GPIO_FREQUENCYMETER_INPUT       // GPIO d'entrée du signal
#define MEASURE_PERIOD_US    1000000 // Période d'échantillonnage (1s) - periode de declenchement de l'IT timer

static gptimer_handle_t gptimer = NULL;
static int pulse_count = 0;
static pcnt_unit_handle_t pcnt_unit = NULL;  // Déclaration du handle PCNT

static bool IRAM_ATTR timer_callback(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx) {
    int count = 0;
    pcnt_unit_get_count(pcnt_unit, &count);
    pulse_count = count;
    pcnt_unit_clear_count(pcnt_unit);  // Réinitialisation du compteur

    meas_t *measure=user_ctx;
    memcpy(measure->pdata, &pulse_count, measure->size);
    measure->ready=true;

    return true;  // Demande au timer de relancer l'interruption
}

void init_pcnt() {
    pcnt_unit_config_t unit_config = {
        .high_limit = 32767,
        .low_limit = -32768,
    };
    ESP_ERROR_CHECK(pcnt_new_unit(&unit_config, &pcnt_unit)); // Correct

    pcnt_chan_config_t chan_config = {
        .edge_gpio_num = PCNT_INPUT_GPIO,
        .level_gpio_num = -1, // Pas de signal de contrôle de niveau
    };
    pcnt_channel_handle_t pcnt_chan = NULL;
    ESP_ERROR_CHECK(pcnt_new_channel(pcnt_unit, &chan_config, &pcnt_chan));

    ESP_ERROR_CHECK(pcnt_channel_set_edge_action(pcnt_chan, 
      PCNT_CHANNEL_EDGE_ACTION_INCREASE,  // Action sur le front montant
      PCNT_CHANNEL_EDGE_ACTION_HOLD       // Action sur le front descendant
    ));

    ESP_ERROR_CHECK(pcnt_unit_enable(pcnt_unit));
    ESP_ERROR_CHECK(pcnt_unit_clear_count(pcnt_unit));
    ESP_ERROR_CHECK(pcnt_unit_start(pcnt_unit));
}

void init_timer(meas_t *measure) 
{
    static void* user_ctx;
    gptimer_config_t timer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = 1*1000*1000,  // 1MHz
    };
    gptimer_new_timer(&timer_config, &gptimer);

    gptimer_event_callbacks_t cbs = {
        .on_alarm = timer_callback,
    };

    measure->handle = (void*) gptimer;
    user_ctx = measure;
    gptimer_register_event_callbacks(gptimer, &cbs, user_ctx);

    gptimer_alarm_config_t alarm_config = {
        .alarm_count = MEASURE_PERIOD_US, // precision du frequencemetre (1Hz)
        .reload_count = 0,
        .flags.auto_reload_on_alarm = true,
    };
    gptimer_set_alarm_action(gptimer, &alarm_config);
    gptimer_enable(gptimer);
    gptimer_start(gptimer);
}

esp_err_t init_frequencymeter(meas_t *measure)
{
   measure->size = sizeof(int);
   measure->pdata = malloc(measure->size * sizeof(uint8_t));
   measure->pdata_cache = malloc(measure->size * sizeof(uint8_t));
   measure->meas_func = NULL;
   
   init_pcnt();
   init_timer(measure);

   return ESP_OK;

}

esp_err_t stop_frequencymeter(meas_t *measure)
{
  static gptimer_handle_t timer;
 
  timer = measure->handle;

  ESP_ERROR_CHECK(gptimer_stop(timer));  // Arrêter le timer
  ESP_ERROR_CHECK(gptimer_disable(timer));
  ESP_ERROR_CHECK(gptimer_del_timer(timer));   // Supprimer le timer

  return ESP_OK;
}

esp_err_t calc_frequencymeter(instance_meas_t *instance_meas)
{
    meas_t measure=instance_meas->measures;
    memset(instance_meas->calc_value, 0, CALC_VALUE_SIZE*sizeof(char));
    int freq=0;

    freq = *(measure.pdata_cache);
    freq |= ((*(measure.pdata_cache+1))<<8)&0xFF00;
    freq |= ((*(measure.pdata_cache+2))<<16)&0xFF0000;
    freq |= ((*(measure.pdata_cache+3))<<24)&0xFF000000;

    sprintf(instance_meas->calc_value,"%d", freq );

    return ESP_OK;
}



