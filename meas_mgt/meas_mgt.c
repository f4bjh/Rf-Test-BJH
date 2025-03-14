//#include <stdio.h>
//#include <assert.h>
#include <string.h>

#include <freertos/FreeRTOS.h>
#include <esp_http_server.h>
#include <freertos/task.h>
#include <esp_ota_ops.h>
#include "esp_log.h"

#include "main.h"
#include "meas_mgt.h"
#include "meas.h"

static const char *TAG = "meas_mgt";

extern const meas_state_t meas_state_pending ;
extern const meas_state_t meas_state_remove ;

TaskHandle_t meas_fsm_tsk_handle=NULL;   

#define INSTANCE_MEAS_DEFAULT()           \
{                                         \
    .current_state = meas_state_pending,  \
    .retries=5,                           \
    .measures.ready = false,              \
    .measures.size  = 0,                  \
    .measures.pdata = NULL,               \
    .measures.pdata_cache = NULL,         \
    .measures.meas_func=NULL,             \
    .json_meas.ready=false,               \
} 

#define LAST_INSTANCE_MEAS {-1,false,NULL,NULL}

instance_meas_per_html_page_t instance_meas_per_html_page[N_PAGES][N_MEAS] = 
{
  //index.html
  {
    //meas_t value,     once,  init_func_hw,         get_calc_func
    {CHIP_NAME,         true,  init_chip_info_model, calc_chip_info_model},
    {CHIP_VERSION,      true,  init_chip_revision,   calc_chip_revision},
    {COUNTER,           false, init_counter,         calc_counter},
    {CURRENT_PARTITION, true,  init_current_part,    calc_current_part},
    LAST_INSTANCE_MEAS
  },
};


size_t get_nb_of_instance_meas(html_page_id_t page_id)
{
  size_t n_meas=0;	

  while(n_meas<N_MEAS) {
    if (instance_meas_per_html_page[page_id][n_meas].meas_num == -1)
      break;
    n_meas++;
  }

  return n_meas;
}

instance_meas_t *meas_mgt_init(html_page_id_t page_id)
{
   instance_meas_default_t instance_meas_default= INSTANCE_MEAS_DEFAULT();
   instance_meas_t *instance_meas, *instance_meas_temp;
   size_t n_meas;
   meas_number_t meas_num;
   meas_action_t meas_action = { .event = MEAS_INIT, .meas_num = 0};
   static meas_fsm_task_arg_t meas_fsm_task_arg;

   n_meas = get_nb_of_instance_meas(page_id);

   instance_meas = malloc((n_meas+1) * sizeof(instance_meas_t));
   instance_meas_temp = instance_meas;

    for (meas_num=0;meas_num<n_meas;instance_meas_temp++,meas_num++) {

      instance_meas_temp->meas_num = instance_meas_per_html_page[page_id][meas_num].meas_num; 
      instance_meas_temp->current_state = instance_meas_default.current_state;
      instance_meas_temp->once = instance_meas_per_html_page[page_id][meas_num].once; 
      instance_meas_temp->retries=instance_meas_default.retries;
      instance_meas_temp->measures.ready= instance_meas_default.measures.ready;
      instance_meas_temp->measures.size= instance_meas_default.measures.size;
      instance_meas_temp->measures.pdata= instance_meas_default.measures.pdata;
      instance_meas_temp->measures.pdata_cache= instance_meas_default.measures.pdata_cache;
      instance_meas_temp->measures.meas_func= instance_meas_default.measures.meas_func;
      instance_meas_temp->json_meas.ready= instance_meas_default.json_meas.ready;
      instance_meas_temp->init_func_hw=instance_meas_per_html_page[page_id][meas_num].init_func_hw;
      instance_meas_temp->calc_func=instance_meas_per_html_page[page_id][meas_num].calc_func;

      instance_meas_temp->q_action = xQueueCreate(1 ,sizeof (meas_action_t)); 
      if (instance_meas_temp->q_action == NULL) {
        ESP_LOGE(TAG,"Failed to create q_action (%d)",meas_num);
	return instance_meas;
      }

      meas_action.event = MEAS_INIT;
      meas_action.meas_num = instance_meas_temp->meas_num;
      if (xQueueSendToBack(instance_meas_temp->q_action, &meas_action, 0) != pdTRUE) {
	ESP_LOGE(TAG,"Failed to send MEAS_INIT to queue (%d)",meas_num);
        return instance_meas;
      }

    }

    //set last measure
    (instance_meas+n_meas)->meas_num = -1;

    //create FSM-measurment on cpu0 fsm_meas_task
    meas_fsm_task_arg.instance_meas=instance_meas;
    meas_fsm_task_arg.n_meas=n_meas;
    xTaskCreate( meas_fsm_task, "measurment fsm ", 4096, &meas_fsm_task_arg, tskFSM_MEASURMENT, &meas_fsm_tsk_handle );

    return instance_meas;
}

esp_err_t instance_meas_remove(instance_meas_t *instance_meas)
{
    instance_meas_t *instance_meas_temp = instance_meas;
    meas_number_t meas_num;

   if (instance_meas == NULL) {
      ESP_LOGE(TAG, "Removing NULL instance_meas");
      return ESP_FAIL;
   }
   
   if (meas_fsm_tsk_handle!=NULL)
     vTaskDelete(meas_fsm_tsk_handle);
	
   while(instance_meas_temp->meas_num!=-1) {

      vQueueDelete( instance_meas_temp->q_action);
      instance_meas_temp->calc_func=NULL;
      instance_meas_temp->init_func_hw=NULL;
      instance_meas_temp->json_meas.ready=false;

      instance_meas_temp->measures.meas_func=NULL;	   
      if (instance_meas_temp->current_state.id != meas_state_remove.id) {
       	if (instance_meas_temp->measures.pdata!=NULL)
          free(instance_meas_temp->measures.pdata);
        if (instance_meas_temp->measures.pdata_cache!=NULL)
          free(instance_meas_temp->measures.pdata_cache);
	if (instance_meas_temp->measures.task_handle!=NULL)
	  vTaskDelete(instance_meas_temp->measures.task_handle);
      }

      instance_meas_temp->measures.size=0;
      instance_meas_temp->measures.ready=false;
      instance_meas_temp->retries=0;
      instance_meas_temp->once = true; 
      instance_meas_temp->current_state = meas_state_remove;
      instance_meas_temp->meas_num = -1;
      instance_meas_temp++; 
    }

    return ESP_OK;
}
