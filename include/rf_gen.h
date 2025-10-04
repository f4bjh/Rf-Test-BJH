#include "adf4351_v2.h"

typedef struct rf_gen_task_arg_s {
  meas_t *measure;
  adf4351_cfg_t vfo;
}rf_gen_task_arg_t;
esp_err_t init_rf_gen(meas_t *measure);
esp_err_t stop_counter(meas_t *measure);
esp_err_t calc_rf_gen(instance_meas_t *instance_meas);
esp_err_t stop_rf_gen(meas_t *measure);
esp_err_t update_rf_gen(meas_t *measure);
