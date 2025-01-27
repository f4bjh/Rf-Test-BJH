typedef enum {
  NO_DATA_TAG,
  CHIP_INFO_MODEL_DATA_TAG,
  CHIP_INFO_REVISION_DATA_TAG
} T_DATA_TAG;

typedef struct json_data_s {
  T_DATA_TAG tag;
  char length;
  char value[32];  
} json_data_t;

void set_default_json_string(char **default_json_string);
esp_err_t data_init(void);
void get_chip_info_model(char *data);
void get_chip_info_revision(char *data);
