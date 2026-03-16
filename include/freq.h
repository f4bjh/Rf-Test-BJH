typedef struct frequencymeter_task_arg_s {
  meas_t *measure;
  reciproc_freq_cfg_t fpga_freq;
}frequencymeter_task_arg_t;

typedef struct read_status_task_arg_s {
  meas_t *measure;
  reciproc_freq_cfg_t fpga_freq;
}read_status_task_arg_t;

esp_err_t init_frequencymeter(meas_t *measure);
esp_err_t calc_frequencymeter(instance_meas_t *instance_meas);
esp_err_t stop_frequencymeter(meas_t *measure);
esp_err_t init_read_status(meas_t *measure);
esp_err_t calc_read_status(instance_meas_t *instance_meas);
esp_err_t stop_read_status(meas_t *measure);

