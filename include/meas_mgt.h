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

typedef enum {
	CHIP_NAME,
	CHIP_VERSION,
	CURRENT_PARTITION,
	NEXT_PARTITION,
	COUNTER,
	CURRENT_PART_VERSION,
	CURRENT_PART_BUILD_DATE,
	NEXT_PART_VERSION,
	NEXT_PART_BUILD_DATE,
	N_MEAS
} meas_number_t;

#define LAST_MEAS N_MEAS

typedef enum {
  MEAS_STATE_PENDING,
  MEAS_STATE_INIT,
  MEAS_STATE_GET,
  MEAS_STATE_CALC,
  MEAS_STATE_FORMAT_JSON,
  MEAS_STATE_PUSH_IN_QUEUE,
  MEAS_STATE_REMOVE,
  N_STATES,
} meas_state_id;

typedef esp_err_t (*meas_state_func)();

// Defining State, holds both state id and state routine
typedef struct {
    meas_state_id id;
    meas_state_func func;
}  meas_state_t;

typedef enum {
  NOEVENT,
  MEAS_INIT,    //event set by http_server task, for all measures of an html page 
  MEAS_PULL,    //event set by fsm task that will poll cpu1 data is measures are ready
  MEAS_PUSH,    //event set by fsm that will push data in http server queue to upload measure to client
  MEAS_REMOVE,  //event set by fsm that will remove measures (in case of error, or because it is pure static measures
  N_EVENTS,
} meas_event_t;

typedef struct {
  meas_event_t event;
  meas_number_t meas_num;
} meas_action_t;

typedef struct meas_s meas_t;
typedef esp_err_t (meas_func_proto)(meas_t *);
typedef meas_func_proto* meas_func_t;

typedef struct meas_s {
  bool ready;   //measure is ready (set by cup1, reset by fsm)
  size_t size;     //size of the measures (set by cpu1)
  uint8_t *pdata;  //pointer to ram with measures result (set and fill by cpu1)
  uint8_t *pdata_cache;  //cache of pdata
  meas_func_t	  meas_func;
} meas_t;

typedef struct {
  bool ready;
  T_DATA_TAG tag;
  char length;
  char value[64];  
  char *json_string;
} json_meas_t;

typedef struct instance_meas_s instance_meas_t;
typedef  esp_err_t (init_func_hw_proto)(meas_t*);
typedef init_func_hw_proto* init_func_hw_t;
typedef  esp_err_t (calc_func_proto)(instance_meas_t *instance_meas);
typedef calc_func_proto* calc_func_t;

typedef struct instance_meas_s {
 meas_number_t meas_num;
 meas_state_t current_state;
 bool once;
 uint8_t retries;
 meas_t measures; //output data from cpu1
 char calc_value[64];  //output string from meas_state_calc_func
 json_meas_t json_meas; //output string from meas_state_format_json_func
 init_func_hw_t init_func_hw;
 calc_func_t calc_func;
 QueueHandle_t q_action;
} instance_meas_t;

typedef struct instance_config_meas_s {
  meas_state_t current_state;
  bool once;
  uint8_t retries;
} instance_config_meas_t;

typedef  esp_err_t (*state_func)(instance_meas_t *);

// State routines to be executed at each state
esp_err_t meas_state_init_func(instance_meas_t *instance_meas);
esp_err_t meas_state_get_func(instance_meas_t *instance_meas);
esp_err_t meas_state_calc_func(instance_meas_t *instance_meas);
esp_err_t meas_state_format_json_func(instance_meas_t *instance_meas);
esp_err_t meas_state_push_in_queue_func(instance_meas_t *instance_meas);
esp_err_t meas_state_pending_func(instance_meas_t *instance_meas);
esp_err_t meas_state_remove_func(instance_meas_t *instance_meas);

instance_meas_t *meas_mgt_init(instance_config_meas_t meas_config);
void meas_fsm_task(void *arg);

//void set_json_data (cJSON *root, json_data_t *json_data);
void set_default_json_string(char **default_json_string);

//to move to aother header file
//void get_chip_info_model(meas_t *measure);
esp_err_t init_chip_info_model(meas_t *measure);
esp_err_t  calc_chip_info_model(instance_meas_t *instance_meas);
esp_err_t instance_meas_remove(instance_meas_t *instance_meas);
