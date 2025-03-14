typedef struct counter_task_arg_s {
  meas_t *measure;
}counter_task_arg_t;

esp_err_t init_counter(meas_t *measure);
esp_err_t  calc_counter(instance_meas_t *instance_meas);

