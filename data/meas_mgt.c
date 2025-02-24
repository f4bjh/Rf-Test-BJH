//#include <stdio.h>
//#include <assert.h>

// Definitions of state id's, event id's, and function pointer
//#define N_STATES  3
//#define N_EVENTS  4


typedef enum {
  STATE_MEAS_PENDING,
  STATE_MEAS_INIT,
  STATE_MEAS_GET,
  STATE_MEAS_CALC,
  STATE_MEAS_FORMAT_JSON,
  STATE_MEAS_PUSH_IN_QUEUE,
  STATE_MEAS_REMOVE,
  STATE_MEAS_ADD,
  N_STATES,
} state_id;

typedef enum {
  NOEVENT,
  INIT_MEAS,    //event set by http_server task, for all measures of an html page 
  PULL_MEAS,    //event set by fsm task that will poll cpu1 data is measures are ready
  PUSH_MEAS,    //event set by fsm that will push data in http server queue to upload measure to client
  REMOVE_MEAS,  //event set by fsm that will remove measures (in case of error, or because it is pure static measures
  N_EVENTS,
} event_t;

typedef struct {
meas_number_t meas_num;
event_t  ev;
} q_msg_t;

typedef struct{
  boolean ready;   //measure is ready (set by cup1, reset by fsm)
  size_t size;     //size of the measures (set by cpu1)
  uint8_t *pdata;  //pointer to ram with measures result (set and ill by cpu1)
} measures_t

struct {
 //meas_number_t meas_num;
 boolean once;
 uint8_t retries;
 measures_t measures;
 char value[];
 char *json_string;
 esp_err_t (*init_func)(instance_data_t *);
 esp_err_t (*calc_func)(instance_data_t *);
} instance_data;

typedef struct instance_data instance_data_t;
typedef  esp_err_t (*state_func)(instance_data_t *);

// Defining State, holds both state id and state routine
typedef struct {
    state_id id;
    state_func func;
}  state_t;

// General functions
void evaluate_state(Event e);

// State routines to be executed at each state
esp_err_t state_meas_init_func(instance_data_t *data);
esp_err_t state_meas_get_func(instance_data_t *data);
esp_err_t state_meas_calc_func(instance_data_t *data);
esp_err_t state_meas_fromat_json_func(instance_data_t *data);
esp_err_t state_meas_push_in_queue_func(instance_data_t *data);
esp_err_t state_meas_pending_func(instance_data_t *data);
esp_err_t state_meas_remove_func(instance_data_t *data);
esp_err_t state_meas_add_func(instance_data_t *data);

// Defining each state with associated state routine
const state_t state_meas_init = {STATE_MEAS_INIT, state_meas_init_func};
const state_t state_meas_get = {STATE_MEAS_GET, state_meas_get_func};
const state_t state_meas_calc = {STATE_MEAS_CALC, state_meas_calc_func};
const state_t state_meas_format_json = {STATE_FORMAT_JSON, state_meas_format_json_func};
const state_t state_meas_push_in_queue = {STATE_PUSH_IN_QUEUE, state_meas_push_in_queue_func};
const state_t state_meas_pending = {STATE_PENDING, state_meas_pending_func};
const state_t state_meas_remove = {STATE_REMOVE, state_meas_remove_func};
const state_t state_meas_add = {STATE_ADD, state_meas_add_func};


// Defning state transition matrix as visualized in the header (events not
// defined, result in mainting the same state)
state_t state_transition_mat[N_STATES][N_EVENTS] = {
   // NO_EVENT,                 INIT_MEAS,                PULL_MEAS,                PUSH_MEAS,                REMOVE_MEAS
   { state_meas_pending,        state_meas_init,          state_meas_get,           state_meas_pending,       state_meas_remove},
   { state_meas_init,           state_meas_init,          state_meas_get,           state_meas_init,          state_meas_remove},
   { state_meas_get,            state_meas_get,           state_meas_get,           state_meas_calc,          state_meas_remove},
   { state_meas_calc,           state_meas_calc,          state_meas_calc,          state_meas_format_json,   state_meas_remove},
   { state_meas_format_json,    state_meas_format_json,   state_meas_format_json,   state_meas_push_in_queue, state_meas_remove},
   { state_meas_push_in_queue,  state_meas_push_in_queue, state_meas_get,           state_meas_push_in_queue, state_meas_remove},
   { state_meas_remove,         state_meas_pending,       state_meas_remove,        state_meas_remove,        state_meas_remove}};
   

T_DATA_TAG get_measurement_tag(instance_data_t *data)
{
T_DATA_TAG result;
  switch (data->meas_num) {
    case DATA_TO_SEND_IS_CHIP_NAME:
      result = CHIP_INFO_MODEL_DATA_TAG;
      break;

  }

  return result;
}


// Define current state and initialize
state_t current_state = state_meas_init;



int fsm_meas_main()
{

  instance_data_t data[LAST_MEAS] = {0};
  //event_t ev;
    q_msg_t q_msg;

  

    while(1) {
    // Event to receive from user
    


    //read the queue
    //get all the event recevied in queue
    //2 bytes : [meas number T_DATA_TO_SEND] - [ event event_t]    perhaps, rename DATA_TO_SEND, in something likie measnumerment number

    
   
      if (xQueueReceive(queue, &q_msg , 0 ) == pdTRUE) {

//		      il y a peut etre plusieurs msg dans la queue, a checker !!


		        evaluate_state(q_msg.ev, &data[q_msg.meas_num]);
     } 
	    
      
    
    };
    return (0);
}

/*
 * Determine state based on event and perform state routine
 */
void evaluate_state(event_t ev, instance_data_t **data)
{
    //Determine state based on event
    current_state = state_transition_mat[current_state.id][ev];
    printf("Transitioned to state: %c\n", current_state.id + 65);
  
    // Run state routine
    (*current_state.func)(*data); //valuer du pointeur vers data a revoir
}

/*
 * State routines
 */

esp_err_t (*state_meas_init)(instance_data_t * data)
{ 
data->init_func(data);

	 q_msg.meas_num = data->meas_num;
     q_msg.ev = PULL_MEAS;
     xQueueSend(queue, &q_msg,0); //a affiner. time to wait ? nom de la queue fsm ? nom q_msg ?
  return ESP_OK;
}

esp_err_t (*state_meas_pending)(instance_data_t * data)
{ 
  q_msg_t q_msg;

  if (data->once) {
     q_msg.meas_num = data->meas_num;
     q_msg.ev = REMOVE_MEAS;
     xQueueSend(queue, &q_msg,0); //a affiner. time to wait ? nom de la queue fsm ? nom q_msg ?
     }
     
  return ESP_OK;
}
     


esp_err_t (*state_meas_get)(instance_data_t * data)
{ 


if (data->measures.ready) {//it has been set by cpu 1
  //copy ram measure zone of cpu1 into data->measures.pdata
    q_msg.meas_num = data->meas_num;
     q_msg.ev = PUSH_MEAS;
     xQueueSend(queue, &q_msg,0); //a affiner. time to wait ? nom de la queue fsm ? nom q_msg ?
}
  
  return ESP_OK;
}

esp_err_t (*state_meas_calc)(instance_data_t * data)
{ 
  data->calc_func(data);
  return ESP_OK;
}

esp_err_t (*state_meas_format_json)(instance_data_t * data)
{ 
     cJSON *root;
   json_data_t json_data; 
  
   sprintf(json_data.value,"%s", data->value);
   json_data.length=strlen(data->json_data.value);
	
  
		if (json_data.length !=0) {

      json_data.tag = get_measurement_tag(data)
      root = cJSON_CreateObject();
		  set_json_data(root,&json_data);
		  data->json_string = cJSON_Print(root);
		  cJSON_Delete(root);
      
		}
  
  return ESP_OK;
}


esp_err_t (*state_meas_push_in_queue)(instance_data_t * data)
{ 

  

   while( (xQueueSend( xQueue, data->json_string, xDelay ) == errQUEUE_FULL) && (retries <data->retries)) {
	   retries++;
	 }

   //on retry on next current state wich will be the same
   if (retries == data-> max_retries) {
     q_msg.meas_num = data->meas_num;
     q_msg.ev = PUSH_MEAS;
     xQueueSend(queue, &q_msg,0); //a affiner. time to wait ? nom de la queue fsm ? nom q_msg ?
   }
   else if (data->once)
      {
     q_msg.meas_num = data->meas_num;
     q_msg.ev = REMOVE_MEAS;
     xQueueSend(queue, &q_msg,0); //a affiner. time to wait ? nom de la queue fsm ? nom q_msg ?
   }
   else {
        q_msg.meas_num = data->meas_num;
     q_msg.ev = PULL_MEAS;
     xQueueSend(queue, &q_msg,0); //a affiner. time to wait ? nom de la queue fsm ? nom q_msg ?
  
   }
     

  

  return ESP_OK;
}


//to be called by http_server
esp_err_t init_meas_fsm(meas_number_t meas_num)
{
     q_msg.meas_num = meas_num;
     q_msg.ev = INIT_MEAS;
     xQueueSend(queue, &q_msg,0); //a affiner. time to wait ? nom de la queue fsm ? nom q_msg ?
}


