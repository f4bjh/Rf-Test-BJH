//5 is the number of element in the queue
//so it is the number of measures (type json_data) that can handled in a time (in 500 ms sampling of get_measurement) 
#define NB_OF_MEASUREMENT_IN_QUEUE  5

// Json string content of one measurment is : 
// + 1 char for {
// + 3 char for \n\t\" + 1 char for "t" + 5 char for \":\t<tag_value>,
// + 3 char for \n\t\" + 1 char for "l" + 5 char for \":\t<length_value>,
// + 3 char for \n\t\" + 1 char for "v" + (4+32+2) char for \":\t\"<string_value[32]>\"\n
// + 1 char for }
#define JSON_STRING_SIZE_OF_ONE_MEASUREMENT (1 + 9 + 9 + 42 + 1) 

#define JSON_STRING_SIZE_OF_MEASUREMENTS NB_OF_MEASUREMENT_IN_QUEUE*JSON_STRING_SIZE_OF_ONE_MEASUREMENT

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
