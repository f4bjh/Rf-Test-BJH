#include "http_server.h"

extern httpd_uri_t index_get;
extern httpd_uri_t about_get;
extern httpd_uri_t frequencymeter_get;
extern httpd_uri_t generator_get;
extern httpd_uri_t powermeter_get;
extern httpd_uri_t upload_get;
extern httpd_uri_t wifi_get;


static const char* TAG = "ws";

esp_err_t open_instance_meas(httpd_handle_t hd, html_page_id_t pageId)
{
    ESP_LOGI(TAG, "New instance meas open %d", pageId);
    server_ctx_t *server_ctx = httpd_get_global_user_ctx(hd);
    instance_meas_t  *instance_meas=NULL;
 
    if (server_ctx == NULL) {
       ESP_LOGE(TAG, "Contexte serveur introuvable");
       return ESP_FAIL;
    }

    if (xSemaphoreTake(server_ctx->mutex, portMAX_DELAY)) {
      instance_meas = server_ctx->instance_meas;
      xSemaphoreGive(server_ctx->mutex);
    } else {
      ESP_LOGE(TAG, "failed to take semaphore to get server_ctx");
      return ESP_FAIL;
    }

    if (instance_meas == NULL) {
      instance_meas = meas_mgt_init(pageId);
    } else {
      ESP_LOGE(TAG,"instance already initialised");
      return ESP_FAIL;
    }


    if (xSemaphoreTake(server_ctx->mutex, portMAX_DELAY)) {
      server_ctx->instance_meas = instance_meas;
      xSemaphoreGive(server_ctx->mutex);
    } else {
      ESP_LOGE(TAG, "failed to take semaphore to set server_ctx");
      return ESP_FAIL;
    }

    return ESP_OK;
}

void close_instance_meas(httpd_handle_t hd)
{
    server_ctx_t *server_ctx = httpd_get_global_user_ctx(hd);
    //wss_keep_alive_t wss_keep_alive;
    instance_meas_t  *instance_meas=NULL;


    ESP_LOGI(TAG, "close meas instance");

    if (xSemaphoreTake(server_ctx->mutex, portMAX_DELAY)) {
      instance_meas = server_ctx->instance_meas;
      xSemaphoreGive(server_ctx->mutex);
    } else {
      ESP_LOGE(TAG, "failed to take semaphore to get server_ctx");
      return;
    }

    instance_meas_remove_all(instance_meas);
	
    if (instance_meas != NULL) 
      free(instance_meas);

    if (xSemaphoreTake(server_ctx->mutex, portMAX_DELAY)) {
      server_ctx->instance_meas = NULL;
      xSemaphoreGive(server_ctx->mutex);
    } else {
      ESP_LOGE(TAG, "failed to take semaphore to set server_ctx");
      return;
    }
}

void ws_process_update_param(httpd_req_t *req, int num, int size, char* param_value)
{
    server_ctx_t *server_ctx = NULL;
    instance_meas_t  *instance_meas=NULL;
    instance_meas_t  *instance_meas_temp=NULL;
    char *endptr;
    uint32_t value;
    esp_err_t ret;

      server_ctx = (server_ctx_t *) httpd_get_global_user_ctx(req->handle);

      if (xSemaphoreTake(server_ctx->mutex, portMAX_DELAY)) {
          instance_meas = server_ctx->instance_meas;
          xSemaphoreGive(server_ctx->mutex);
      } else {
          ESP_LOGE(TAG, "failed to take semaphore to get instance_meas in server_ctx");
          return;
      }

      //convert char param_value into meas_param_in[8];
      switch (meas_num) {
        case RF_GEN_STATUS:
          if ((strncmp(param_value,rf_gen_on, strlen(rf_gen_on))==0))
	    instance_meas->measures.meas_param_in[0] = 1;
	  else
 	    instance_meas->measures.meas_param_in[0] = 0;
	  break;
        case RF_GEN_FREQ:
          value = strtoul(param_value, NULL, 10); // Base 10
          memcpy(&(instance_meas->measures.meas_param_in[1]), &value, 4);
	  break;
        case RF_GEN_POW:
	  //TODO
	  //fill measures.meas_param_in[5] with power value 0,1,2,3 but param_value is a char. strtol should make the trick
	  break;
	default:
	  break;


      }

      ret = instance_meas->measures.meas_update_func(&(instance_meas->measures));
      if (ret != ESP_OK)
        ESP_LOGE(TAG, "error in meas_update (%d)",meas_num);

}

void ws_process_received_page_id(httpd_req_t *req, int size, char* received_page_id)
{
  const char *page_name;
  int pageId;


    //TODO : if l=0, it is index.html also
    //need to revert old commit that changed uri name into index.html also

    page_name=&(index_get.uri[1]);
    if ((strncmp(received_page_id,page_name, strlen(page_name))==0) || 
         (size==0)) {
	pageId = INDEX_HTML_PAGE_ID; //define ou enum;
    }
    page_name=&(upload_get.uri[1]);
    if (strncmp(received_page_id,page_name, strlen(page_name))==0) {
        pageId = UPLOAD_HTML_PAGE_ID; //define ou enum;
    }
    page_name=&(frequencymeter_get.uri[1]);
    if (strncmp(received_page_id,page_name, strlen(page_name))==0) {
        pageId = FREQUENCYMETER_HTML_PAGE_ID; //define ou enum;
    }
    page_name=&(generator_get.uri[1]);
    if (strncmp(received_page_id,page_name, strlen(page_name))==0) {
        pageId = GENERATOR_HTML_PAGE_ID; //define ou enum;
    }

    open_instance_meas(req->handle, pageId);
    return ;

}

void ws_process_received_json(httpd_req_t *req, httpd_ws_frame_t ws_pkt)
{

           cJSON *root = cJSON_Parse((char *)ws_pkt.payload);

           if (!root) {
		ESP_LOGE(TAG, "failed to parse received JSON");
	   }else {

	        // Extraction des valeurs
	        cJSON *t = cJSON_GetObjectItem(root, "t");
	        cJSON *l = cJSON_GetObjectItem(root, "l");
	        cJSON *v = cJSON_GetObjectItem(root, "v");
		//TODO : shall we delete root or not ?
		//cJSON_Delete(root);

	        ESP_LOGI(TAG, "Received packet with message (fd=%d): t: %d, l: %d, v: %s", httpd_req_to_sockfd(req), t->valueint, l->valueint, v->valuestring);

		switch (t->valueint) {
  	  	  case PAGE_ID_TAG:
		    ws_process_received_page_id(req,l->valueint,v->valuestring);
		    break;
  		  case RF_GEN_STATUS_TAG:
		    ws_process_update_param(req, RF_GEN_STATUS,l->valueint, v->valuestring);
		    break;
		  case RF_GEN_FREQ_TAG:
		    ws_process_update_param(req, RF_GEN_FREQ, l->valueint, v->valuestring);
		    break;
		  case RF_GEN_LEVEL_TAG:
		    ws_process_update_param(req, RF_GEN_POW, l->valueint, v->valuestring);
		    break;
		  default:
		    break;
		}
	   } 
}


//handle and trigged by an HTTP request
static esp_err_t ws_handler(httpd_req_t *req)
{
    
    ESP_LOGI(TAG, "ws_handler: httpd_handle_t=%p, sockfd=%d, client_info:%d", req->handle,
                 httpd_req_to_sockfd(req), httpd_ws_get_fd_info(req->handle, httpd_req_to_sockfd(req)));

    if (req->method == HTTP_GET) {
        ESP_LOGI(TAG, "Handshake done, new connection opened (fd=%d)", httpd_req_to_sockfd(req));
        return ESP_OK;
    }
    httpd_ws_frame_t ws_pkt;
    uint8_t *buf = NULL;
    memset(&ws_pkt, 0, sizeof(httpd_ws_frame_t));

    // First receive the full ws message
    /* Set max_len = 0 to get the frame len */
    esp_err_t ret = httpd_ws_recv_frame(req, &ws_pkt, 0);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "httpd_ws_recv_frame failed to get frame len with %d", ret);
        return ret;
    }
    
    if (ws_pkt.len) {
        /* ws_pkt.len + 1 is for NULL termination as we are expecting a string */
        buf = calloc(1, ws_pkt.len + 1);
        if (buf == NULL) {
            ESP_LOGE(TAG, "Failed to calloc memory for buf");
            return ESP_ERR_NO_MEM;
        }
        ws_pkt.payload = buf;
        /* Set max_len = ws_pkt.len to get the frame payload */
        ret = httpd_ws_recv_frame(req, &ws_pkt, ws_pkt.len);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "httpd_ws_recv_frame failed with %d", ret);
            free(buf);
            return ret;
        }
    }

    // If it was a PONG, update the keep-alive
    if (ws_pkt.type == HTTPD_WS_TYPE_PONG) {
        ESP_LOGI(TAG, "Received PONG message (fd=%d)",httpd_req_to_sockfd(req));
        free(buf);
	server_ctx_t *h_meas = httpd_get_global_user_ctx(req->handle);
        wss_keep_alive_t h = h_meas->keep_alive;

	return wss_keep_alive_client_is_active(h,httpd_req_to_sockfd(req));

    } else if (ws_pkt.type == HTTPD_WS_TYPE_TEXT || ws_pkt.type == HTTPD_WS_TYPE_PING || ws_pkt.type == HTTPD_WS_TYPE_CLOSE) {
        if (ws_pkt.type == HTTPD_WS_TYPE_TEXT) {

          ws_process_received_json(req, ws_pkt);

	  //client is waiting data only in json format
	  //response TEXT with no payload
	  ws_pkt.len = 0;
          ws_pkt.payload = NULL;

	} else if (ws_pkt.type == HTTPD_WS_TYPE_PING) {
            // Response PONG packet to peer
            ESP_LOGI(TAG, "Got a WS PING frame, Replying PONG");
            ws_pkt.type = HTTPD_WS_TYPE_PONG;
        } else if (ws_pkt.type == HTTPD_WS_TYPE_CLOSE) {
            // Response CLOSE packet with no payload to peer
	    close_instance_meas(req->handle);
            ws_pkt.len = 0;
            ws_pkt.payload = NULL;
	}
        
	ret = httpd_ws_send_frame(req, &ws_pkt);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "httpd_ws_send_frame failed with %d", ret);
        }
    }
    free(buf);
    return ret;
}
httpd_uri_t ws = {
        .uri        = "/ws",
        .method     = HTTP_GET,
        .handler    = ws_handler,
        .user_ctx   = NULL,
        .is_websocket = true,
        .handle_ws_control_frames = true
};

void ws_async_send(void *arg)
{
    struct async_resp_arg *resp_arg = arg;
    httpd_handle_t hd = resp_arg->hd;
    int fd = resp_arg->fd;
    httpd_ws_frame_t ws_pkt;
    char *json_string;
    instance_meas_t  *instance_meas=resp_arg->instance_meas;

#ifndef DISABLE_WDT_TASK
    esp_task_wdt_status(NULL);
#endif


  if (instance_meas!=NULL) {
    while(instance_meas->meas_num!=LAST_MEAS) {

      if (instance_meas->json_meas.ready) {

        json_string = instance_meas->json_meas.json_string;
	    
	    memset(&ws_pkt, 0, sizeof(httpd_ws_frame_t));
        ws_pkt.payload = (uint8_t*)json_string;
        ws_pkt.len = strlen(json_string);
        ws_pkt.type = HTTPD_WS_TYPE_TEXT;
        ESP_LOGI(TAG, "send async (%p) %d bytes of data to ws client (fd=%d)", json_string, ws_pkt.len, fd);
        ESP_LOGV(TAG, "%s",ws_pkt.payload);
        esp_err_t err = httpd_ws_send_frame_async(hd, fd, &ws_pkt);

	    if (err != ESP_OK) {
    		ESP_LOGE(TAG, "failed to send WebSocket message: %s", esp_err_to_name(err));
	    }
        instance_meas->json_meas.ready=false;
      }
      instance_meas++;
    } 
  }	
  assert(resp_arg!=NULL);
  free(resp_arg);

}

esp_err_t ws_open_fd(httpd_handle_t hd, int sockfd)
{
    server_ctx_t *server_ctx = httpd_get_global_user_ctx(hd);
    wss_keep_alive_t wss_keep_alive;

    ESP_LOGI(TAG, "New client connected %d", sockfd);
 
    wss_keep_alive = server_ctx->keep_alive;
    return wss_keep_alive_add_client(wss_keep_alive, sockfd);
}

void ws_close_fd(httpd_handle_t hd, int sockfd)
{
    server_ctx_t *server_ctx = httpd_get_global_user_ctx(hd);
    wss_keep_alive_t wss_keep_alive;

    ESP_LOGI(TAG, "Client disconnected %d", sockfd);
    wss_keep_alive = server_ctx->keep_alive;
    wss_keep_alive_remove_client(wss_keep_alive, sockfd);
    close(sockfd);
}
