#include <string.h>

#include <freertos/FreeRTOS.h>
#include <esp_http_server.h>
#include <freertos/task.h>
#include <esp_ota_ops.h>
#include "esp_log.h"

#include "meas_mgt.h"

static const char *TAG = "meas_fsm";

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


int meas_fsm_task(void *arg)
{

  instance_meas_t *instance_meas = arg;
  meas_action_t meas_action;

    while(1) {    
      xQueueReceive(instance_meas->q_action, (void*)&meas_action , 0 );
      evaluate_state(meas_action.event,&instance_meas[meas_action.meas_num]);
    };
	
    return (0);
}

/*
 * State routines
 */

esp_err_t meas_state_init_func(instance_meas_t *instance_meas)
{
    meas_action_t meas_action = { .event = MEAS_PULL, .meas_num = meas_num};
    esp_err_t err;
 
	
  if ( instance_meas->init_func_hw == NULL) {
    ESP_LOGI(TAG,"no init_func_hw for instance measure %d", instacen_meas->meas_num);    
  } else if (instance_meas->init_func_hw(instance_meas)!=ESP_OK) {
    ESP_LOGE(TAG,"init_func_hw error for instance measure %d", instacen_meas->meas_num);    
  }

  if (xQueueSendToBack(instance_meas->q_action, &meas_action, 0) == pdTRUE) {
        return ESP_OK;
    }
  return ESP_FAIL;

}

esp_err_t meas_state_get_func(instance_meas_t *instance_meas)
{
    meas_action_t meas_action = { .event = MEAS_PUSH, .meas_num = meas_num};
    esp_err_t err;
 
	
  if (instance_meas->mesures.ready) {
	  //copy instance_mease->measures.pdata into instance_meas->measures.pdata_cache
	  instance_meas->mesures.ready = false;
  }
 
  if (xQueueSendToBack(instance_meas->q_action, &meas_action, 0) == pdTRUE) {
       return ESP_OK;
  }
  return ESP_FAIL;

}


esp_err_t meas_state_calc_func(instance_meas_t *instance_meas)
{ 
    meas_action_t meas_action = { .event = MEAS_PUSH, .meas_num = meas_num};
    esp_err_t err;
 
	
  if ( instance_meas->calc_func == NULL) {
    ESP_LOGI(TAG,"no calc_func for instance measure %d", instacen_meas->meas_num);    
  } else if (instance_meas->calc_func(instance_meas)!=ESP_OK) {
    ESP_LOGE(TAG,"init_func_hw error for instance measure %d", instacen_meas->meas_num);    
  }

  if (xQueueSendToBack(instance_meas->q_action, &meas_action, 0) == pdTRUE) {
        return ESP_OK;
    }
  return ESP_FAIL;

}
esp_err_t meas_state_format_json_func(instance_meas_t *instance_meas)
{

  meas_action_t meas_action = { .event = MEAS_PUSH, .meas_num = meas_num};
  esp_err_t err;

	
        //cop string instance_meas->calc_value into  instance_meas->json_meas.value
	instance_meas->json_meas.length=strlen(instance_meas->json_meas.value);

       if (instance_meas->json_meas.length !=0) {

	  instance_meas->json_meas.tag = CHIP_INFO_MODEL_DATA_TAG;//should be the same value as in the switch case
	  root = cJSON_CreateObject();
	  
	   cJSON_AddNumberToObject(root, "t",  instance_meas->json_meas.tag);
           cJSON_AddNumberToObject(root, "l", instance_meas->json_meas.length);
           cJSON_AddStringToObject(root, "v", instance_meas->json_meas.value);

	  instance_meas->json_meas.json_string = cJSON_Print(root);
	  cJSON_Delete(root);
      }

     if (instance_meas->once) {
	 meas_action.event=MEAS_REMOVE;
     }
  
     if (xQueueSendToBack(instance_meas->q_action, &meas_action, 0) == pdTRUE) {
        return ESP_OK;
    }
    return ESP_FAIL;

	
}

esp_err_t meas_state_push_in_queue_func(instance_meas_t *instance_meas)
{
  meas_action_t meas_action = { .event = MEAS_PULL, .meas_num = meas_num};
  esp_err_t err;
  
	//xQueueSendToBack(instance_meas->q_json_string_meas, instance_meas->json_meas.json_string,250ms && retries<instance_meas->retries)
   
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
  return ESP_OK;
}

esp_err_t meas_state_remove_func(instance_meas_t *instance_meas)
{
  if ( instance_meas->json_meas.measures || instance_meas->json_meas.ready) {
    instance_meas->json_meas.measures=false;
    instance_meas->json_meas.ready=false;

    ESP_LOGI(TAG,"instance measure %d has been removed", instance_meas->meas_num);
  }
 
  return ESP_OK;
}


