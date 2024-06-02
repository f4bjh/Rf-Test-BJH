typedef struct json_data_s {
  T_DATA_TAG tag;
  char length;
  char *value;  
} json_data_t;

typedef enum {
  UNUSED_DATA_TAG,
  CHIP_INFO_MODEL_DATA_TAG,
  CHIP_INFO_REVISION_DATA_TAG
} T_DATA_TAG;

void data_init(void);
