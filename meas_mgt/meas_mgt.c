#include <string.h>
#include <freertos/FreeRTOS.h>
#include <esp_http_server.h>
#include <freertos/task.h>
#include <esp_ota_ops.h>
#include "esp_log.h"

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

#define LAST_INSTANCE_MEAS {LAST_MEAS,false,NULL,NULL,{0},NULL,NULL,NULL}

instance_meas_per_html_page_t instance_meas_per_html_page[N_PAGES][N_MEAS+1] = 
{
  //index.html
  {
    //meas_t value,           once,  init_func_hw,                 get_calc_func,                meas_param_in, some handle, meas_stop_func,      meas_stop_func
    {CHIP_NAME,               true,  init_chip_info_model,         calc_chip_info_model,         {0},           NULL,        NULL,                NULL},
    {CHIP_VERSION,            true,  init_chip_revision,           calc_chip_revision,           {0},           NULL,        NULL,                NULL},
    {CPU0_COUNTER,            false, init_counter,                 calc_counter,                 {0},           NULL,        stop_counter,        NULL},
    {CPU1_COUNTER,            false, init_counter,                 calc_counter,                 {1},           NULL,        stop_counter,        NULL},
    {CURRENT_PARTITION,       true,  init_current_part,            calc_current_part,            {0},           NULL,        NULL,                NULL},
    {NEXT_PARTITION,          true,  init_next_part,               calc_next_part,               {0},           NULL,        NULL,                NULL},
    {CURRENT_PART_VERSION,    true,  init_current_part_version,    calc_current_part_version,    {0},           NULL,        NULL,                NULL},
    {CURRENT_PART_BUILD_DATE, true,  init_current_part_build_date, calc_current_part_build_date, {0},           NULL,        NULL,                NULL},
    {NEXT_PART_VERSION,       true,  init_next_part_version,       calc_next_part_version,       {0},           NULL,        NULL,                NULL},
    {NEXT_PART_BUILD_DATE,    true,  init_next_part_build_date,    calc_next_part_build_date,    {0},           NULL,        NULL,                NULL},
    LAST_INSTANCE_MEAS
  },
  //generator.html
  {
	  //param default to set : rf status off + 100MHz + 5dBm
    {RF_GEN_STATUS,           false, init_rf_gen,                  calc_rf_gen,                  {0,0,0,0,0,3},  NULL,        stop_rf_gen,         update_rf_gen},
#if 0
    {RF_GEN_FREQ,             false, NULL,                         NULL,                         {0},           NULL,        NULL,                update_rf_gen},
    {RF_GEN_POW,              false, NULL,                         NULL,                         {0},           NULL,        NULL,                update_rf_gen},
#endif
    LAST_INSTANCE_MEAS
  },
  //frequencymeter.html
  {
    {FREQUENCY,               false, init_frequencymeter,          calc_frequencymeter,          {0},           NULL,        stop_frequencymeter,  NULL},
    LAST_INSTANCE_MEAS
  },
  //powermeter.html
  {
    LAST_INSTANCE_MEAS
  },
  //upload.html
  {
    {CURRENT_PARTITION,       true,  init_current_part,            calc_current_part, {0},                      NULL,        NULL,                 NULL},
    {NEXT_PARTITION,          true,  init_next_part,               calc_next_part,    {0},                      NULL,        NULL,                 NULL},
    LAST_INSTANCE_MEAS
  }, 
};


size_t get_nb_of_instance_meas(html_page_id_t page_id)
{
  size_t n_meas=0;	

  while(n_meas<N_MEAS) {
    if (instance_meas_per_html_page[page_id][n_meas].meas_num == LAST_MEAS)
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
   uint8_t index;

   n_meas = get_nb_of_instance_meas(page_id);
   ESP_LOGV(TAG,"initialise %d measurment",n_meas);

   instance_meas = malloc((n_meas+1) * sizeof(instance_meas_t));
   instance_meas_temp = instance_meas;

    for (meas_num=0;meas_num<=n_meas;instance_meas_temp++,meas_num++) {

      instance_meas_temp->meas_num = instance_meas_per_html_page[page_id][meas_num].meas_num; 
      instance_meas_temp->current_state = instance_meas_default.current_state;
      instance_meas_temp->once = instance_meas_per_html_page[page_id][meas_num].once; 
      instance_meas_temp->retries=instance_meas_default.retries;
      instance_meas_temp->measures.ready= instance_meas_default.measures.ready;
      instance_meas_temp->measures.size= instance_meas_default.measures.size;
      instance_meas_temp->measures.pdata= instance_meas_default.measures.pdata;
      instance_meas_temp->measures.pdata_cache= instance_meas_default.measures.pdata_cache;
      instance_meas_temp->measures.meas_func= instance_meas_default.measures.meas_func;
      for (index=0; index<8; index++)
        instance_meas_temp->measures.meas_param_in[index] = instance_meas_per_html_page[page_id][meas_num].meas_param_in[index];
      instance_meas_temp->measures.handle = instance_meas_per_html_page[page_id][meas_num].handle;
      instance_meas_temp->measures.meas_stop_func = instance_meas_per_html_page[page_id][meas_num].meas_stop_func;
      instance_meas_temp->measures.meas_update_func = instance_meas_per_html_page[page_id][meas_num].meas_update_func;
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

    //create FSM-measurment on cpu0 fsm_meas_task
    meas_fsm_task_arg.instance_meas=instance_meas;
    meas_fsm_task_arg.n_meas=n_meas;
    xTaskCreatePinnedToCore( meas_fsm_task, "measurment fsm ", 4096, &meas_fsm_task_arg, tskFSM_MEASURMENT, &meas_fsm_tsk_handle,0 );

    return instance_meas;
}

esp_err_t instance_meas_remove(instance_meas_t *instance_meas)	   
{

      if (instance_meas == NULL) {
	ESP_LOGE(TAG,"try to remove NULL instance meas");
	return ESP_FAIL;
      }
	
      if (instance_meas->meas_num==REMOVED_MEAS) {
	ESP_LOGE(TAG,"instance meas %p already removed", instance_meas);
	return ESP_FAIL;
      }
	
      vQueueDelete( instance_meas->q_action);
      instance_meas->calc_func=NULL;
      instance_meas->init_func_hw=NULL;
      instance_meas->json_meas.ready=false;

      instance_meas->measures.meas_update_func = NULL;
        
      if (instance_meas->measures.meas_stop_func != NULL) {
    	  instance_meas->measures.meas_stop_func(&(instance_meas->measures));
	  instance_meas->measures.meas_stop_func = NULL;
      }

      instance_meas->measures.meas_func=NULL;	
      if (instance_meas->measures.pdata_cache!=NULL)
        free(instance_meas->measures.pdata_cache);
      if (instance_meas->measures.pdata!=NULL)
        free(instance_meas->measures.pdata);
  
      instance_meas->measures.size=0;
      instance_meas->measures.ready=false;
      instance_meas->retries=0;
      instance_meas->once = true; 
      instance_meas->current_state = meas_state_remove;
      instance_meas->meas_num = REMOVED_MEAS;
       

    return ESP_OK;
}

esp_err_t instance_meas_remove_all(instance_meas_t *instance_meas)
{
    instance_meas_t *instance_meas_temp = instance_meas;
    
   if (instance_meas == NULL) {
      ESP_LOGE(TAG, "Removing NULL instance_meas");
      return ESP_FAIL;
   }
   
   if (meas_fsm_tsk_handle!=NULL)
     vTaskDelete(meas_fsm_tsk_handle);
	
   while(instance_meas_temp->meas_num!=LAST_MEAS) {
    instance_meas_remove(instance_meas_temp);
    instance_meas_temp++;
   }

   return ESP_OK;
}
