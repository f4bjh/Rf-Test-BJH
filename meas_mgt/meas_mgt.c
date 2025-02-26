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


   
#if 0
T_DATA_TAG get_measurement_tag(instance_data_t *data)
{

  T_DATA_TAG result;

  switch (data->meas_num) {

    case DATA_TO_SEND_IS_CHIP_NAME:
      result = CHIP_INFO_MODEL_DATA_TAG;
      break;
    default:
      result = NO_DATA_TAG;
      break;
  }

  return result;
}
#endif

init_func_hw_t get_init_func_hw(meas_number_t meas_num)
{
  init_func_hw_t func;

  switch(meas_num) {
    case CHIP_NAME:
      func=init_chip_info_model; 
      break;
    default:
      func=NULL;
      break;
#if 0
	CHIP_VERSION,
	CURRENT_PARTITION,
	NEXT_PARTITION,
	COUNTER,
	CURRENT_PART_VERSION,
	CURRENT_PART_BUILD_DATE,
	NEXT_PART_VERSION,
	NEXT_PART_BUILD_DATE,
#endif


  }
  return func;
}

calc_func_t get_calc_func(meas_number_t meas_num)
{
  calc_func_t func;

  switch(meas_num) {
    case CHIP_NAME:
      func=calc_chip_info_model;
      break;
    default:
      func=NULL;
      break;
#if 0
	CHIP_VERSION,
	CURRENT_PARTITION,
	NEXT_PARTITION,
	COUNTER,
	CURRENT_PART_VERSION,
	CURRENT_PART_BUILD_DATE,
	NEXT_PART_VERSION,
	NEXT_PART_BUILD_DATE,
#endif


  }
  return func;
}
/*
cette fonction a un double emploi
  d'abord initialise et lance la tache
  et surtout pointe sur des valeur des struc instance data_t
  sera appele par cpu0, mais devra etre passee a cpu1
  a voir s'il faut une fonction qui libere instance_meas + arrête la tache fsm
    et qui serait appellee quand le client se deconnecte
  du coup meas_init devrait sans doute etre appelee dans ws_handler de http server
  ws handler va devoir recuper le pageId (=index ? = powermeter ?, etc...) et c'est peut etre
  ws_handler qui va devoir appeler la fonction qui fait MEAS_ADD
*/
instance_meas_t *meas_mgt_init(instance_config_meas_t meas_config)
{
    meas_number_t meas_num;
	
    instance_meas_t *instance_meas = malloc(N_MEAS * sizeof(instance_meas_t));

    for (meas_num=0;meas_num<N_MEAS;meas_num++) {
      (instance_meas+meas_num)->meas_num = meas_num; 
      (instance_meas+meas_num)->current_state = config.current_state;
      (instance_meas+meas_num)->once = config.once; 
      (instance_meas+meas_num)->retries=config.retries;
      //instance_meas[meas_num].measures; a voir
      //instance_meas[meas_num].json_meas; a voir
      (instance_meas+meas_num)->init_func_hw=get_init_func_hw(meas_num);
      (instance_meas+meas_num)->calc_func= get_calc_func(meas_num);

      (instance_meas+meas_num)->q_action = xQueueCreate(1 ,sizeof (meas_action_t)); 
      if ((instance_meas+meas_num)->q_action == NULL) {
        ESP_LOGE(TAG,"Failed to create q_action (%d)",meas_num);
        return ESP_FAIL;
      }
    }
 
    //create FSM-measurment on cpu0 fsm_meas_task
    xTaskCreate( meas_fsm_task, "measurment fsm ", 4096, data, tskFSM_MEASURMENT, NULL );

    return instance_meas;
}

/*
  callback to be called by http server
*/
//to be called by cpu1

//cpu1 aura accees a la struc instance_data_t
//car cpu0 (qui aura lance l'init meas_init, lui aura passe le pointeur aussi
//a voir comment on fait d'ailleurs
//
esp_err_t meas_mgt_meas_init_cb(instance_meas_t *instance_meas, meas_number_t meas_num)
{
	
    meas_action_t meas_action = { .event = MEAS_INIT, .meas_num = meas_num};

    if (xQueueSendToBack(instance_meas->q_action, &meas_action, 0) == pdTRUE) {
        return ESP_OK;
    }
    return ESP_FAIL;

}

/*
commnet http_server recupere q_out
on doit pouvoir passer le pointeyr de instance_data_t dans les arg de ws_async_send
et du coup dans ws_async_send : xQueueRceive (data->q_out
*/
