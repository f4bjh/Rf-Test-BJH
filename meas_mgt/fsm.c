#include <string.h>

#include <freertos/FreeRTOS.h>
#include <esp_http_server.h>
#include <freertos/task.h>
#include <esp_ota_ops.h>
#include "esp_log.h"

#include "meas_mgt.h"
#include "cJSON.h"

static const char *TAG = "meas_fsm";

// Defining each state with associated state routine
const meas_state_t meas_state_pending = {MEAS_STATE_PENDING, meas_state_pending_func};
const meas_state_t meas_state_init = {MEAS_STATE_INIT, meas_state_init_func};
const meas_state_t meas_state_get = {MEAS_STATE_GET, meas_state_get_func};
const meas_state_t meas_state_calc = {MEAS_STATE_CALC, meas_state_calc_func};
const meas_state_t meas_state_format_json = {MEAS_STATE_FORMAT_JSON, meas_state_format_json_func};
const meas_state_t meas_state_push_in_queue = {MEAS_STATE_PUSH_IN_QUEUE, meas_state_push_in_queue_func};
const meas_state_t meas_state_remove = {MEAS_STATE_REMOVE, meas_state_remove_func};

meas_state_t state_transition_mat[N_STATES][N_EVENTS] = {
   // NO_EVENT,                 MEAS_INIT,                MEAS_PULL,                MEAS_PUSH,                MEAS_REMOVE
   { meas_state_pending,        meas_state_init,          meas_state_get,           meas_state_pending,       meas_state_remove},
   { meas_state_init,           meas_state_init,          meas_state_get,           meas_state_init,          meas_state_remove},
   { meas_state_get,            meas_state_get,           meas_state_get,           meas_state_calc,          meas_state_remove},
   { meas_state_calc,           meas_state_calc,          meas_state_calc,          meas_state_format_json,   meas_state_remove},
   { meas_state_format_json,    meas_state_format_json,   meas_state_format_json,   meas_state_push_in_queue, meas_state_remove},
   { meas_state_push_in_queue,  meas_state_push_in_queue, meas_state_get,           meas_state_push_in_queue, meas_state_remove},
   { meas_state_remove,         meas_state_remove,        meas_state_remove,        meas_state_remove,        meas_state_remove}};
   

/*
 * Determine state based on event and perform state routine
 */
void evaluate_state(meas_event_t ev, instance_meas_t *instance_meas)
{

    esp_err_t  err;

    //Determine state based on event
    instance_meas->current_state = state_transition_mat[instance_meas->current_state.id][ev];
  
    // Run state routine
    err = (*(instance_meas->current_state.func))(instance_meas); 
    if (err != ESP_OK)
      ESP_LOGE(TAG,"run state routine error");
}


void meas_fsm_task(void *arg)
{
  meas_fsm_task_arg_t *meas_fsm_task_arg = arg;
  instance_meas_t *instance_meas = meas_fsm_task_arg->instance_meas;
  instance_meas_t *instance_meas_temp=instance_meas;
  size_t  n_meas = meas_fsm_task_arg->n_meas;
  meas_action_t meas_action;
  meas_number_t meas_num;


    while(1) {   
     
      for (meas_num=0, instance_meas_temp=instance_meas;meas_num<n_meas;instance_meas_temp++,meas_num++) {
        vTaskDelay(10 / portTICK_PERIOD_MS); 

        if (xQueueReceive(instance_meas_temp->q_action, (void*)&meas_action , 0 ) == pdTRUE) {
          evaluate_state(meas_action.event,instance_meas_temp);
        }
      }
    };
	
}

/*
 * State routines
 */

esp_err_t meas_state_init_func(instance_meas_t *instance_meas)
{
    meas_action_t meas_action = { .event = MEAS_PULL, .meas_num = instance_meas->meas_num};

      ESP_LOGI(TAG,"initialize measure %d", instance_meas->meas_num);

      if ( instance_meas->init_func_hw == NULL) {
        ESP_LOGI(TAG,"init_func_hw not defined for instance measure %d", instance_meas->meas_num);   
        instance_meas->measures.meas_func=NULL;
	meas_action.event=MEAS_REMOVE;
      } else if (instance_meas->init_func_hw(&(instance_meas->measures))!=ESP_OK) {
        ESP_LOGE(TAG,"init_func_hw error for instance measure %d", instance_meas->meas_num);    
      }

      //si once=false, executer la tache sur CPU1, sauf s'elle a deja ete creee

      if (xQueueSendToBack(instance_meas->q_action, &meas_action, 0) == pdTRUE) {
        return ESP_OK;
      }
  
      return ESP_FAIL;
}

esp_err_t meas_state_get_func(instance_meas_t *instance_meas)
{
  meas_action_t meas_action = { .event = MEAS_PUSH, .meas_num = instance_meas->meas_num};
  esp_err_t err=ESP_OK;

  if (instance_meas->once) {
    if (instance_meas->measures.meas_func) {
	instance_meas->measures.meas_func(&(instance_meas->measures));
    } else {
	ESP_LOGE(TAG,"meas_func not defined for %d", instance_meas->meas_num);
	meas_action.event=MEAS_REMOVE;
    }
  }

  if (instance_meas->measures.ready) {
   
    ESP_LOGI(TAG,"measure %d ready", instance_meas->meas_num);

    memcpy(instance_meas->measures.pdata_cache, instance_meas->measures.pdata, instance_meas->measures.size);
    instance_meas->measures.ready = false;

    
  } else 
    meas_action.event=MEAS_PULL;
 
  if (xQueueSendToBack(instance_meas->q_action, &meas_action, 0) != pdTRUE) 
        err=ESP_FAIL;

  return err;

}


esp_err_t meas_state_calc_func(instance_meas_t *instance_meas)
{ 
  meas_action_t meas_action = { .event = MEAS_PUSH, .meas_num = instance_meas->meas_num};

  ESP_LOGI(TAG,"calculate measure %d", instance_meas->meas_num);

  if ( instance_meas->calc_func == NULL) {
    ESP_LOGI(TAG,"no calc_func for instance measure %d", instance_meas->meas_num);    
  } else if (instance_meas->calc_func(instance_meas)!=ESP_OK) {
    ESP_LOGE(TAG,"calc_func error for instance measure %d", instance_meas->meas_num);    
  }

  if (xQueueSendToBack(instance_meas->q_action, &meas_action, 0) == pdTRUE) {
        return ESP_OK;
    }
  return ESP_FAIL;

}

T_DATA_TAG get_tag_measurement(meas_number_t meas_num)
{
 T_DATA_TAG tag;

  switch (meas_num) {
    case CHIP_NAME:
      tag = CHIP_INFO_MODEL_DATA_TAG;
      break;
    case CHIP_VERSION:
      tag = CHIP_INFO_REVISION_DATA_TAG;
      break;
    case COUNTER:
      tag = COUNTER_VALUE_TAG;
      break;
    default:
      tag = NO_DATA_TAG;
      break;
  }

  return tag;

}


esp_err_t meas_state_format_json_func(instance_meas_t *instance_meas)
{

  meas_action_t meas_action = { .event = MEAS_PUSH, .meas_num = instance_meas->meas_num};
  cJSON *root;
   
      ESP_LOGI(TAG,"format in json measure %d %s", instance_meas->meas_num, instance_meas->calc_value);

      strncpy( instance_meas->json_meas.value, instance_meas->calc_value, sizeof(instance_meas->json_meas.value) - 1);
      instance_meas->json_meas.value[sizeof(instance_meas->json_meas.value) - 1] = '\0'; // Ensure null-termination

      instance_meas->json_meas.length=strlen(instance_meas->json_meas.value);

      if (instance_meas->json_meas.length !=0) {

	  instance_meas->json_meas.tag = get_tag_measurement(instance_meas->meas_num);
	  root = cJSON_CreateObject();
	  
	  cJSON_AddNumberToObject(root, "t",  instance_meas->json_meas.tag);
          cJSON_AddNumberToObject(root, "l", instance_meas->json_meas.length);
          cJSON_AddStringToObject(root, "v", instance_meas->json_meas.value);

	  instance_meas->json_meas.json_string = cJSON_Print(root);
	  cJSON_Delete(root);
     }

     if (xQueueSendToBack(instance_meas->q_action, &meas_action, 0) == pdTRUE) {
        return ESP_OK;
    }
    return ESP_FAIL;

	
}

esp_err_t meas_state_push_in_queue_func(instance_meas_t *instance_meas)
{
  meas_action_t meas_action = { .event = MEAS_PULL, .meas_num = instance_meas->meas_num};
  
    ESP_LOGI(TAG,"measure %d json ready", instance_meas->meas_num);
 
    instance_meas->json_meas.ready=true;

    if (instance_meas->once) {
	 meas_action.event=MEAS_REMOVE;
    }
  
    if (xQueueSendToBack(instance_meas->q_action, &meas_action, 0) == pdTRUE) {
        return ESP_OK;
    }
    return ESP_FAIL;
}

esp_err_t meas_state_pending_func(instance_meas_t *instance_meas)
{
  ESP_LOGI(TAG,"measure %d in pending state", instance_meas->meas_num);
  return ESP_OK;
}

esp_err_t meas_state_remove_func(instance_meas_t *instance_meas)
{

  if (instance_meas->measures.pdata!=NULL)
    free(instance_meas->measures.pdata);
  if (instance_meas->measures.pdata_cache!=NULL)
    free(instance_meas->measures.pdata_cache);
  if (instance_meas->measures.task_handle!=NULL)
    vTaskDelete(instance_meas->measures.task_handle);
 
  ESP_LOGI(TAG,"instance measure %d has been removed", instance_meas->meas_num);

  return ESP_OK;
}


