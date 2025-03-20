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

