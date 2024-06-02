typedef enum {
  UNUSED_DATA_TAG,
  CHIP_INFO_MODEL_DATA_TAG,
  CHIP_INFO_REVISION_DATA_TAG
} T_DATA_TAG;

typedef struct json_data_s {
  T_DATA_TAG tag;
  char length;
  char *value;  
} json_data_t;

esp_err_t data_init(void);
void get_chip_info_model(char *data, char *length);
