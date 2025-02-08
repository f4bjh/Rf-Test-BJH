#include "cJSON.h"

//number of element in the queue that content json measurment value
#define NB_OF_MEASUREMENT_IN_QUEUE  10

// Json string content of one measurment is : 
// + 1 char for {
// + 3 char for \n\t\" + 1 char for "t" + 5 char for \":\t<tag_value>,
// + 3 char for \n\t\" + 1 char for "l" + 5 char for \":\t<length_value>,
// + 3 char for \n\t\" + 1 char for "v" + (4+32+2) char for \":\t\"<string_value[32]>\"\n
// + 1 char for }
#define JSON_STRING_SIZE_OF_ONE_MEASUREMENT (1 + 9 + 9 + 42 + 1) 

#define JSON_STRING_SIZE_OF_MEASUREMENTS NB_OF_MEASUREMENT_IN_QUEUE*JSON_STRING_SIZE_OF_ONE_MEASUREMENT

#define GET_MEASURMENT_DELAY 10 //10ms

typedef enum {
  NO_DATA_TAG,
  CHIP_INFO_MODEL_DATA_TAG,
  CHIP_INFO_REVISION_DATA_TAG,
  CURRENT_PARTITION_NAME_TAG,
  NEXT_PARTITION_TAG,
  COUNTER_VALUE_TAG,
  CURRENT_PART_VERSION_TAG,
  CURRENT_PART_BUILD_DATE_TAG,
  NEXT_PART_VERSION_TAG,
  NEXT_PART_BUILD_DATE_TAG,
} T_DATA_TAG;

typedef struct json_data_s {
  T_DATA_TAG tag;
  char length;
  char value[64];  
} json_data_t;

typedef enum {
	DATA_TO_SEND_IS_CHIP_NAME,
	DATA_TO_SEND_IS_CHIP_VERSION,
	DATA_TO_SEND_IS_CURRENT_PARTITION,
	DATA_TO_SEND_IS_NEXT_PARTITION,
	DATA_TO_SEND_IS_COUNTER,
	DATA_TO_SEND_IS_CURRENT_PART_VERSION,
	DATA_TO_SEND_IS_CURRENT_PART_BUILD_DATE,
	DATA_TO_SEND_IS_NEXT_PART_VERSION,
	DATA_TO_SEND_IS_NEXT_PART_BUILD_DATE,
	DATA_TO_SEND_IS_LAST
} T_DATA_TO_SEND;

void set_json_data (cJSON *root, json_data_t *json_data);
void set_default_json_string(char **default_json_string);
esp_err_t data_init(void);
void get_chip_info_model(char *data);
void get_chip_info_revision(char *data);
