#include "http_server.h"

/*
 * HTTP Server
 */
static const char* TAG = "http_server";

static const size_t max_clients = 4;

extern const uint8_t style_css_start[] asm("_binary_style_css_start");
extern const uint8_t style_css_end[] asm("_binary_style_css_end");
extern const uint8_t index_html_start[] asm("_binary_index_html_start");
extern const uint8_t index_html_end[] asm("_binary_index_html_end");
extern httpd_uri_t about_get;
extern httpd_uri_t frequencymeter_get;
extern const uint8_t generator_html_start[] asm("_binary_generator_html_start");
extern const uint8_t generator_html_end[] asm("_binary_generator_html_end");
extern const uint8_t jquery_gauge_css_start[] asm("_binary_jquery_gauge_css_start");
extern const uint8_t jquery_gauge_css_end[] asm("_binary_jquery_gauge_css_end");
extern const uint8_t jquery_gauge_js_start[] asm("_binary_jquery_gauge_js_start");
extern const uint8_t jquery_gauge_js_end[] asm("_binary_jquery_gauge_js_end");
extern const uint8_t jquery_gauge_min_js_start[] asm("_binary_jquery_gauge_min_js_start");
extern const uint8_t jquery_gauge_min_js_end[] asm("_binary_jquery_gauge_min_js_end");
extern const uint8_t jquery_min_js_start[] asm("_binary_jquery_min_js_start");
extern const uint8_t jquery_min_js_end[] asm("_binary_jquery_min_js_end");
extern const uint8_t powermeter_html_start[] asm("_binary_powermeter_html_start");
extern const uint8_t powermeter_html_end[] asm("_binary_powermeter_html_end");
extern const uint8_t script_js_start[] asm("_binary_script_js_start");
extern const uint8_t script_js_end[] asm("_binary_script_js_end");
extern httpd_uri_t upload_get;
extern httpd_uri_t update_post ;
extern httpd_uri_t reboot_after_upload_post;
extern httpd_uri_t wifi_get;
extern httpd_uri_t set_wifi_uri_handler;
extern httpd_uri_t reboot_post ;

extern TaskHandle_t xHandle_keep_alive;

TaskHandle_t xHandle_ws_send_msg;

struct async_resp_arg {
    httpd_handle_t hd;
    int fd;
    instance_meas_t *instance_meas;
};

static httpd_handle_t http_server = NULL;

void ngx_unescape_uri(u_char **dst, u_char **src, size_t size, unsigned int type)
{
    u_char  *d, *s, ch, c, decoded;
    enum {
        sw_usual = 0,
        sw_quoted,
        sw_quoted_second
    } state;

    d = *dst;
    s = *src;

    state = 0;
    decoded = 0;

    while (size--) {
        ch = *s++;
        switch (state) {
        case sw_usual:
            if (ch == '?'
                    && (type & (NGX_UNESCAPE_URI | NGX_UNESCAPE_REDIRECT))) {
                *d++ = ch;
                goto done;
            }
            if (ch == '%') {
                state = sw_quoted;
                break;
            }
            *d++ = ch;
            break;
        case sw_quoted:
            if (ch >= '0' && ch <= '9') {
                decoded = (u_char) (ch - '0');
                state = sw_quoted_second;
                break;
            }
            c = (u_char) (ch | 0x20);
            if (c >= 'a' && c <= 'f') {
                decoded = (u_char) (c - 'a' + 10);
                state = sw_quoted_second;
                break;
            }
            /* the invalid quoted character */
            state = sw_usual;
            *d++ = ch;
            break;

        case sw_quoted_second:
            state = sw_usual;
            if (ch >= '0' && ch <= '9') {
                ch = (u_char) ((decoded << 4) + (ch - '0'));
                if (type & NGX_UNESCAPE_REDIRECT) {
                    if (ch > '%' && ch < 0x7f) {
                        *d++ = ch;
                        break;
                    }
                    *d++ = '%'; *d++ = *(s - 2); *d++ = *(s - 1);
                    break;
                }
                *d++ = ch;
                break;
            }
            c = (u_char) (ch | 0x20);
            if (c >= 'a' && c <= 'f') {
                ch = (u_char) ((decoded << 4) + (c - 'a') + 10);
                if (type & NGX_UNESCAPE_URI) {
                    if (ch == '?') {
                        *d++ = ch;
                        goto done;
                    }
                    *d++ = ch;
                    break;
                }
                if (type & NGX_UNESCAPE_REDIRECT) {
                    if (ch == '?') {
                        *d++ = ch;
                        goto done;
                    }
                    if (ch > '%' && ch < 0x7f) {
                        *d++ = ch;
                        break;
                    }
                    *d++ = '%'; *d++ = *(s - 2); *d++ = *(s - 1);
                    break;
                }
                *d++ = ch;
                break;
            }
            /* the invalid quoted character */
            break;
        }
    }

done:
    *dst = d;
    *src = s;
}

void example_uri_decode(char *dest, const char *src, size_t len)
{
    if (!src || !dest) {
        return;
    }

    unsigned char *src_ptr = (unsigned char *)src;
    unsigned char *dst_ptr = (unsigned char *)dest;
    ngx_unescape_uri(&dst_ptr, &src_ptr, len, NGX_UNESCAPE_URI);
}


static void send_ping(void *arg)
{
    struct async_resp_arg *resp_arg = arg;
    httpd_handle_t hd = resp_arg->hd;
    int fd = resp_arg->fd;
    httpd_ws_frame_t ws_pkt;
    memset(&ws_pkt, 0, sizeof(httpd_ws_frame_t));
    ws_pkt.payload = NULL;
    ws_pkt.len = 0;
    ws_pkt.type = HTTPD_WS_TYPE_PING;
    ESP_LOGI(TAG, "send_ping to fd=%d", fd);
 
    httpd_ws_send_frame_async(hd, fd, &ws_pkt);
    free(resp_arg);
}

bool client_not_alive_cb(wss_keep_alive_t h, int fd)
{
    ESP_LOGE(TAG, "Client not alive, closing fd %d", fd);
    httpd_sess_trigger_close(wss_keep_alive_get_user_ctx(h), fd);
    return true;
}

bool check_client_alive_cb(wss_keep_alive_t h, int fd)
{

  if (httpd_ws_get_fd_info(http_server, fd) == HTTPD_WS_CLIENT_WEBSOCKET) {

    ESP_LOGI(TAG, "Checking if client (fd=%d) is alive - send ping", fd);
    struct async_resp_arg *resp_arg = malloc(sizeof(struct async_resp_arg));
    assert(resp_arg != NULL);
    resp_arg->hd = wss_keep_alive_get_user_ctx(h);
    resp_arg->fd = fd;

    if (httpd_queue_work(resp_arg->hd, send_ping, resp_arg) == ESP_OK){ 
        return true;
    }
  } else {
    ESP_LOGI(TAG, "Client (fd=%d) is not a ws client", fd);
  }

  return false;
    
}

esp_err_t style_get_handler(httpd_req_t *req)
{
	httpd_resp_set_type(req, "text/css");
	httpd_resp_send(req, (const char *) style_css_start, style_css_end - style_css_start);
	return ESP_OK;
}

esp_err_t index_get_handler(httpd_req_t *req)
{

    httpd_resp_send(req, (const char *) index_html_start, index_html_end - index_html_start);
    return ESP_OK;
}
httpd_uri_t index_get = {
	.uri	  = "/index.html",
	.method   = HTTP_GET,
	.handler  = index_get_handler,
	.user_ctx = NULL
};
httpd_uri_t index2_get = {
	.uri	  = "/",
	.method   = HTTP_GET,
	.handler  = index_get_handler,
	.user_ctx = NULL
};

esp_err_t generator_get_handler(httpd_req_t *req)
{
	httpd_resp_send(req, (const char *) generator_html_start, generator_html_end - generator_html_start);
	return ESP_OK;
}

esp_err_t jquery_gauge_css_get_handler(httpd_req_t *req)
{
	httpd_resp_set_type(req, "text/css");
	httpd_resp_send(req, (const char *) jquery_gauge_css_start, jquery_gauge_css_end - jquery_gauge_css_start);
	return ESP_OK;
}

esp_err_t jquery_gauge_js_get_handler(httpd_req_t *req)
{
	httpd_resp_set_type(req, "text/javascript");
	httpd_resp_send(req, (const char *) jquery_gauge_js_start, jquery_gauge_js_end - jquery_gauge_js_start);
	return ESP_OK;
}

esp_err_t jquery_gauge_min_js_get_handler(httpd_req_t *req)
{
	httpd_resp_set_type(req, "text/javascript");
	httpd_resp_send(req, (const char *) jquery_gauge_min_js_start, jquery_gauge_min_js_end - jquery_gauge_min_js_start);
	return ESP_OK;
}

esp_err_t jquery_min_js_get_handler(httpd_req_t *req)
{
	httpd_resp_set_type(req, "text/javascript");
	httpd_resp_send(req, (const char *) jquery_min_js_start, jquery_min_js_end - jquery_min_js_start);
	return ESP_OK;
}

esp_err_t script_js_get_handler(httpd_req_t *req)
{
	httpd_resp_set_type(req, "text/javascript");
	httpd_resp_send(req, (const char *) script_js_start, script_js_end - script_js_start);
	return ESP_OK;
}

esp_err_t powermeter_get_handler(httpd_req_t *req)
{
	httpd_resp_send(req, (const char *) powermeter_html_start, powermeter_html_end - powermeter_html_start);
	return ESP_OK;
}

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



static void ws_async_send(void *arg)
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

// Get all clients and send async message
void ws_server_send_data(void* arg)
{
    bool send_messages = true;
    httpd_handle_t* server=arg;
    server_ctx_t *server_ctx = NULL;
    instance_meas_t  *instance_meas=NULL;

#ifndef DISABLE_WDT_TASK
    esp_task_wdt_add(NULL);
#endif


    // Send async message to all connected clients that use websocket protocol every 10 seconds
    while (send_messages) {


	vTaskDelay(HTTP_SERVER_WAKE_UP_TICK / portTICK_PERIOD_MS);


	if (!*server) { // httpd might not have been created by now
	    continue;
	}

        server_ctx = (server_ctx_t *) httpd_get_global_user_ctx(*server);

        if (xSemaphoreTake(server_ctx->mutex, portMAX_DELAY)) {
          instance_meas = server_ctx->instance_meas;
          xSemaphoreGive(server_ctx->mutex);
        } else {
          ESP_LOGE(TAG, "failed to take semaphore to get instance_meas in server_ctx");
          continue;
        }

	size_t clients = max_clients;
	int    client_fds[max_clients];
	if (httpd_get_client_list(*server, &clients, client_fds) == ESP_OK) {
	    for (size_t i=0; i < clients; ++i) {
		int sock = client_fds[i];
		if (httpd_ws_get_fd_info(*server, sock) == HTTPD_WS_CLIENT_WEBSOCKET) {
		    struct async_resp_arg *resp_arg = malloc(sizeof(struct async_resp_arg));
		    assert(resp_arg != NULL);
		    resp_arg->hd = *server;
		    resp_arg->fd = sock;
		    resp_arg->instance_meas = instance_meas;
		    if (httpd_queue_work(resp_arg->hd,ws_async_send, resp_arg) != ESP_OK) {
			ESP_LOGE(TAG, "httpd_queue_work failed!");
			send_messages = false;
			break;
		    }
		}
	    }
	} else {
	    ESP_LOGE(TAG, "httpd_get_client_list failed!");
	    return;
	}
    }

}

esp_err_t wss_open_fd(httpd_handle_t hd, int sockfd)
{
    server_ctx_t *server_ctx = httpd_get_global_user_ctx(hd);
    wss_keep_alive_t wss_keep_alive;

    ESP_LOGI(TAG, "New client connected %d", sockfd);
 
    wss_keep_alive = server_ctx->keep_alive;
    return wss_keep_alive_add_client(wss_keep_alive, sockfd);
}

void wss_close_fd(httpd_handle_t hd, int sockfd)
{
    server_ctx_t *server_ctx = httpd_get_global_user_ctx(hd);
    wss_keep_alive_t wss_keep_alive;

    ESP_LOGI(TAG, "Client disconnected %d", sockfd);
    wss_keep_alive = server_ctx->keep_alive;
    wss_keep_alive_remove_client(wss_keep_alive, sockfd);
    close(sockfd);
}

httpd_uri_t style_get = {
	.uri	  = "/style.css",
	.method   = HTTP_GET,
	.handler  = style_get_handler,
	.user_ctx = NULL
};

httpd_uri_t generator_get = {
	.uri	  = "/generator.html",
	.method   = HTTP_GET,
	.handler  = generator_get_handler,
	.user_ctx = NULL
};

httpd_uri_t powermeter_get = {
	.uri	  = "/powermeter.html",
	.method   = HTTP_GET,
	.handler  = powermeter_get_handler,
	.user_ctx = NULL
};

httpd_uri_t jquery_gauge_css_get = {
	.uri	  = "/jquery.gauge.css",
	.method   = HTTP_GET,
	.handler  = jquery_gauge_css_get_handler,
	.user_ctx = NULL
};

httpd_uri_t jquery_gauge_js_get = {
	.uri	  = "/jquery.gauge.js",
	.method   = HTTP_GET,
	.handler  = jquery_gauge_js_get_handler,
	.user_ctx = NULL
};

httpd_uri_t jquery_gauge_min_js_get = {
	.uri	  = "/jquery.gauge.min.js",
	.method   = HTTP_GET,
	.handler  = jquery_gauge_min_js_get_handler,
	.user_ctx = NULL
};

httpd_uri_t jquery_min_js_get = {
	.uri	  = "/jquery.min.js",
	.method   = HTTP_GET,
	.handler  = jquery_min_js_get_handler,
	.user_ctx = NULL
};

httpd_uri_t script_js_get = {
	.uri	  = "/script.js",
	.method   = HTTP_GET,
	.handler  = script_js_get_handler,
	.user_ctx = NULL
};

esp_err_t http_server_init(void)
{
	//static httpd_handle_t http_server = NULL;
        server_ctx_t *server_ctx = calloc(1, sizeof(server_ctx_t));
	server_ctx->mutex = xSemaphoreCreateMutex();
	server_ctx->instance_meas=NULL;

	// Prepare keep-alive engine
	wss_keep_alive_config_t keep_alive_config = KEEP_ALIVE_CONFIG_DEFAULT();
	keep_alive_config.max_clients = max_clients;
	keep_alive_config.client_not_alive_cb = client_not_alive_cb;
	keep_alive_config.check_client_alive_cb = check_client_alive_cb;
	wss_keep_alive_t keep_alive = wss_keep_alive_start(&keep_alive_config);
	server_ctx->keep_alive = keep_alive;

	httpd_config_t config = HTTPD_DEFAULT_CONFIG();

	//number of files(html, css, and js) to handle in the http server
	//fix to 255 to have some margin
	config.max_uri_handlers = 255;

	config.max_open_sockets = max_clients;
	config.global_user_ctx = server_ctx;
	config.open_fn = wss_open_fd;
	config.close_fn = wss_close_fd;
	config.task_priority = tskHTTP_SERVER;

	if (httpd_start(&http_server, &config) == ESP_OK) {
		httpd_register_uri_handler(http_server, &style_get);
		httpd_register_uri_handler(http_server, &index_get);
		httpd_register_uri_handler(http_server, &index2_get);
		httpd_register_uri_handler(http_server, &about_get);
		httpd_register_uri_handler(http_server, &frequencymeter_get);
		httpd_register_uri_handler(http_server, &generator_get);
		httpd_register_uri_handler(http_server, &jquery_gauge_css_get);
		httpd_register_uri_handler(http_server, &jquery_gauge_js_get);
		httpd_register_uri_handler(http_server, &jquery_gauge_min_js_get);
		httpd_register_uri_handler(http_server, &jquery_min_js_get);
		httpd_register_uri_handler(http_server, &powermeter_get);
		httpd_register_uri_handler(http_server, &script_js_get);
		httpd_register_uri_handler(http_server, &upload_get);
        httpd_register_uri_handler(http_server, &update_post);
		httpd_register_uri_handler(http_server, &reboot_after_upload_post);
		httpd_register_uri_handler(http_server, &wifi_get);
        httpd_register_uri_handler(http_server, &reboot_post);
        httpd_register_uri_handler(http_server, &set_wifi_uri_handler);
		httpd_register_uri_handler(http_server, &ws);
		wss_keep_alive_set_user_ctx(keep_alive, http_server);
	}
	//ws_server_send_data(&http_server);
	  xTaskCreatePinnedToCore(ws_server_send_data, 
		   "ws server send data", 
		   configMINIMAL_STACK_SIZE, 
		   &http_server,
		   tskHTTP_SERVER,
		   &xHandle_ws_send_msg,
		   0);

	return http_server == NULL ? ESP_FAIL : ESP_OK;
}
