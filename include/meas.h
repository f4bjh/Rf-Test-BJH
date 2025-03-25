#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "esp_system.h"
#include <esp_http_server.h>
#include <sys/param.h>
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "driver/gptimer.h"
#include "driver/pulse_cnt.h"

#include "meas_mgt.h"
#include "gpio.h"
#include "adf4351.h"

esp_err_t init_chip_info_model(meas_t *measure);
esp_err_t  calc_chip_info_model(instance_meas_t *instance_meas);
esp_err_t init_chip_revision(meas_t *measure);
esp_err_t  calc_chip_revision(instance_meas_t *instance_meas);

typedef struct counter_task_arg_s {
  meas_t *measure;
}counter_task_arg_t;

esp_err_t init_counter(meas_t *measure);
esp_err_t  calc_counter(instance_meas_t *instance_meas);
esp_err_t  stop_counter(meas_t *measure);

esp_err_t init_current_part(meas_t *measure);
esp_err_t calc_current_part(instance_meas_t *instance_meas);

esp_err_t init_next_part(meas_t *measure);
esp_err_t calc_next_part(instance_meas_t *instance_meas);

esp_err_t init_current_part_version(meas_t *measure);
esp_err_t calc_current_part_version(instance_meas_t *instance_meas);

esp_err_t init_current_part_build_date(meas_t *measure);
esp_err_t calc_current_part_build_date(instance_meas_t *instance_meas);

esp_err_t init_next_part_version(meas_t *measure);
esp_err_t calc_next_part_version(instance_meas_t *instance_meas);

esp_err_t init_next_part_build_date(meas_t *measure);
esp_err_t calc_next_part_build_date(instance_meas_t *instance_meas);

esp_err_t init_frequencymeter(meas_t *measure);
esp_err_t calc_frequencymeter(instance_meas_t *instance_meas);
esp_err_t stop_frequencymeter(meas_t *measure);

typedef struct rf_gen_task_arg_s {
  meas_t *measure;
  ADF4351_cfg_t vfo;
}rf_gen_task_arg_t;
esp_err_t init_rf_gen(meas_t *measure);
esp_err_t stop_counter(meas_t *measure);
esp_err_t calc_rf_gen(instance_meas_t *instance_meas);
esp_err_t stop_rf_gen(meas_t *measure);
esp_err_t update_rf_gen(meas_t *measure);
