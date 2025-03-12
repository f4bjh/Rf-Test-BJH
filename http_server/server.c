#include <string.h>

#include <freertos/FreeRTOS.h>
#include <esp_http_server.h>
#include <freertos/task.h>
#include <esp_ota_ops.h>
#include <sys/param.h>
#include <esp_http_server.h>
#include "lwip/sockets.h"
#include "esp_log.h"
#include "keep_alive.h"
#include "esp_task_wdt.h"
#include <nvs_flash.h>
#include "esp_check.h"

#include "main.h"
#include "meas_mgt.h"
#include "http_server.h"

extern const meas_state_t meas_state_pending ;
extern const meas_state_t meas_state_get ; //TO DEL

#define MEAS_MGT_ONCE_CONFIG_DEFAULT()    \
    {                                     \
    .current_state = meas_state_pending,  \
    .once = true,                         \
    .retries=5,                           \
} 

/*
 * HTTP Server
 */
static const char* TAG = "http_server";

static const size_t max_clients = 4;

extern const uint8_t style_css_start[] asm("_binary_style_css_start");
extern const uint8_t style_css_end[] asm("_binary_style_css_end");
extern const uint8_t index_html_start[] asm("_binary_index_html_start");
extern const uint8_t index_html_end[] asm("_binary_index_html_end");
extern const uint8_t about_html_start[] asm("_binary_about_html_start");
extern const uint8_t about_html_end[] asm("_binary_about_html_end");
extern const uint8_t frequencymeter_html_start[] asm("_binary_frequencymeter_html_start");
extern const uint8_t frequencymeter_html_end[] asm("_binary_frequencymeter_html_end");
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
extern const uint8_t upload_html_start[] asm("_binary_upload_html_start");
extern const uint8_t upload_html_end[] asm("_binary_upload_html_end");
extern const uint8_t wifi_html_start[] asm("_binary_wifi_html_start");
extern const uint8_t wifi_html_end[] asm("_binary_wifi_html_end");

extern TaskHandle_t xHandle_keep_alive;

struct async_resp_arg {
    httpd_handle_t hd;
    int fd;
    instance_meas_t *instance_meas;
};

static httpd_handle_t http_server = NULL;

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

#if 0	
    server_ctx_t *server_ctx = (server_ctx_t *) httpd_get_global_user_ctx(req->handle);
    int pageId = 0; //define ou enum;

    if (server_ctx == NULL) {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Erreur interne");
        return ESP_FAIL;
    }

    // Protection de l'accès à measure_index
    if (xSemaphoreTake(server_ctx->mutex, portMAX_DELAY)) {
        server_ctx->pageId = pageId;
        xSemaphoreGive(server_ctx->mutex);
    } else {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Erreur de synchronisation");
        return ESP_FAIL;
    }
#endif

    httpd_resp_send(req, (const char *) index_html_start, index_html_end - index_html_start);
    return ESP_OK;
}

esp_err_t about_get_handler(httpd_req_t *req)
{
	httpd_resp_send(req, (const char *) about_html_start, about_html_end - about_html_start);
	return ESP_OK;
}

esp_err_t frequencymeter_get_handler(httpd_req_t *req)
{
	httpd_resp_send(req, (const char *) frequencymeter_html_start, frequencymeter_html_end - frequencymeter_html_start);
	return ESP_OK;
}

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

esp_err_t upload_get_handler(httpd_req_t *req)
{
	httpd_resp_send(req, (const char *) upload_html_start, upload_html_end - upload_html_start);
	return ESP_OK;
}

esp_err_t wifi_get_handler(httpd_req_t *req)
{
	httpd_resp_send(req, (const char *) wifi_html_start, wifi_html_end - wifi_html_start);
	return ESP_OK;
}

/*
 * Handle OTA file upload
 */
esp_err_t update_post_handler(httpd_req_t *req)
{
	char buf[1000];
	esp_ota_handle_t ota_handle;
	int remaining = req->content_len;

	const esp_partition_t *ota_partition = esp_ota_get_next_update_partition(NULL);
	ESP_LOGI(TAG,"Received fw update request - download into partition: %s", ota_partition->label);

       	vTaskSuspend( xHandle_keep_alive );

	if (!ota_partition) {
	 ESP_LOGE(TAG, "no ota partition found");
	 return ESP_ERR_NOT_FOUND;
 	}

	ESP_ERROR_CHECK(esp_ota_begin(ota_partition, OTA_SIZE_UNKNOWN, &ota_handle));

	while (remaining > 0) {
		int recv_len = httpd_req_recv(req, buf, MIN(remaining, sizeof(buf)));

		// Timeout Error: Just retry
		if (recv_len == HTTPD_SOCK_ERR_TIMEOUT) {
			continue;

		// Serious Error: Abort OTA
		} else if (recv_len <= 0) {
			httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Protocol Error");
			return ESP_FAIL;
		}

		// Successful Upload: Flash firmware chunk
		if (esp_ota_write(ota_handle, (const void *)buf, recv_len) != ESP_OK) {
			httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Flash Error");
			return ESP_FAIL;
		}

		remaining -= recv_len;
	}

	// Validate new OTA image
	if (esp_ota_end(ota_handle) != ESP_OK) {
			httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Validation new OTA image  Error");
			return ESP_FAIL;
	}

	//httpd_resp_sendstr(req, "Firmware update complete.\n");
	ESP_LOGI(TAG,"Firmware update complete on %s", ota_partition->label);
	vTaskResume (xHandle_keep_alive);
	
	return ESP_OK;
}


esp_err_t reboot_post_handler(httpd_req_t *req)
{
	const esp_partition_t *ota_partition = esp_ota_get_next_update_partition(NULL);
	if (esp_ota_set_boot_partition(ota_partition) != ESP_OK) {
		httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to set boot partition to next");
		return ESP_FAIL;
	}

	ESP_LOGI(TAG,"Reboot requested to %s!!!", ota_partition->label);
	esp_restart();

	return ESP_OK;
}

esp_err_t open_instance_meas(httpd_handle_t hd, int pageId)
{
    ESP_LOGI(TAG, "New instance meas open");
    server_ctx_t *server_ctx = httpd_get_global_user_ctx(hd);
    instance_meas_t  *instance_meas=NULL;
    instance_config_meas_t meas_config= MEAS_MGT_ONCE_CONFIG_DEFAULT();

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
      instance_meas = meas_mgt_init(meas_config);
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

#if 0
    wss_keep_alive = server_ctx->keep_alive;
    wss_keep_alive_remove_client(wss_keep_alive, sockfd);
#endif

    if (xSemaphoreTake(server_ctx->mutex, portMAX_DELAY)) {
      instance_meas = server_ctx->instance_meas;
      xSemaphoreGive(server_ctx->mutex);
    } else {
      ESP_LOGE(TAG, "failed to take semaphore to get server_ctx");
      return;
    }

    instance_meas_remove(instance_meas);
    if (instance_meas != NULL) 
      free(instance_meas);

    if (xSemaphoreTake(server_ctx->mutex, portMAX_DELAY)) {
      server_ctx->instance_meas = NULL;
      xSemaphoreGive(server_ctx->mutex);
    } else {
      ESP_LOGE(TAG, "failed to take semaphore to set server_ctx");
      return;
    }

//  close(sockfd);
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

    // If it was a TEXT message, just echo it back
    } else if (ws_pkt.type == HTTPD_WS_TYPE_TEXT || ws_pkt.type == HTTPD_WS_TYPE_PING || ws_pkt.type == HTTPD_WS_TYPE_CLOSE) {
        if (ws_pkt.type == HTTPD_WS_TYPE_TEXT) {
            ESP_LOGI(TAG, "Received packet with message (fd=%d): %s",httpd_req_to_sockfd(req), ws_pkt.payload);

           //here, will have handle received payload
	   //it can be a new instance meas to create (based on payload value, wich will contain the pageId)
	   //but it can also be some control of hardware 
	   //(for example, output power of generator, or window time precision of frequencymeter, etc...)

           int pageId = 0; //define ou enum;

	   open_instance_meas(req->handle, pageId);

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


static void ws_async_send(void *arg)
{
    struct async_resp_arg *resp_arg = arg;
    httpd_handle_t hd = resp_arg->hd;
    int fd = resp_arg->fd;
    httpd_ws_frame_t ws_pkt;
    char *json_string;
    meas_number_t meas_num=0;
    instance_meas_t  *instance_meas=resp_arg->instance_meas;

#ifndef DISABLE_WDT_TASK
    esp_task_wdt_status(NULL);
#endif


  if (instance_meas!=NULL) {
    while(meas_num<N_MEAS) {

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
      meas_num++;
    } 
  }	
  assert(resp_arg!=NULL);
  free(resp_arg);

}

// Get all clients and send async message
static void ws_server_send_data(httpd_handle_t* server)
{
    bool send_messages = true;
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

// HTTP request handler for setting Wi-Fi credentials
static esp_err_t set_wifi_post_handler(httpd_req_t *req)
{

    char buf[128];
    int ret;
    char param[EXAMPLE_HTTP_QUERY_KEY_MAX_LEN] = {0};
    char ssid[32]= {0};
    char password[64] = {0};
    uint8_t wifi_credentials_set = WIFI_CREDENTIAL_NOT_SET_IN_FLASH;
    size_t buf_len = req->content_len;
    char *ptr;

	if (buf_len>128) {
            httpd_resp_send(req, "Invalid Wi-Fi credentials", HTTPD_RESP_USE_STRLEN);
	    return ESP_FAIL;
	}

        // Read the data for the request
        if ((ret = httpd_req_recv(req, buf, buf_len)) <= 0) {
            if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
                httpd_resp_send_408(req);
            }
            return ESP_FAIL;
        }

        // Process the received data

	ptr = malloc((buf_len+1) * sizeof(char));
	strncpy(ptr,buf,buf_len);
	ptr[buf_len]='\0';

	httpd_query_key_value(ptr, "ssid", param, 32); 
        example_uri_decode(ssid, param, strnlen(param, EXAMPLE_HTTP_QUERY_KEY_MAX_LEN));

	httpd_query_key_value(ptr, "password", param, 64); 
        example_uri_decode(password, param, strnlen(param, EXAMPLE_HTTP_QUERY_KEY_MAX_LEN));
	
	free(ptr);

 	if (strlen(ssid)>32) {
            httpd_resp_send(req, "Invalid ssid", HTTPD_RESP_USE_STRLEN);
	    return ESP_FAIL;
	}
	if (strlen(password)>64) {
            httpd_resp_send(req, "Invalid password", HTTPD_RESP_USE_STRLEN);
	    return ESP_FAIL;
	}


	// Set Wi-Fi credentials
        wifi_credentials_set = WIFI_CREDENTIAL_SET_IN_FLASH;
        nvs_handle_t my_handle;
        esp_err_t err = nvs_open("storage", NVS_READWRITE, &my_handle);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Error (%s) opening NVS handle!\n", esp_err_to_name(err));
        } else {
            err = nvs_set_str(my_handle, NVS_KEY_SSID, ssid);
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "Error (%s) writing ssid\n", esp_err_to_name(err));
            }
            err = nvs_set_str(my_handle, NVS_KEY_PASSWORD, password);
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "Error (%s) writing password\n", esp_err_to_name(err));
            }
	    err = nvs_set_u8(my_handle, NVS_KEY_WIFI_SET_CREDENTIAL, wifi_credentials_set);
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "Error (%s) writing wifi_credentials_set\n", esp_err_to_name(err));
            }

            nvs_close(my_handle);

        }
       
       	if (err != ESP_OK) 
	  httpd_resp_send(req, "Wi-Fi credentials set unsuccessfully in flash", HTTPD_RESP_USE_STRLEN);
	else
	  httpd_resp_send(req, "Wi-Fi credentials set successfully", HTTPD_RESP_USE_STRLEN);

        return err; 
}

httpd_uri_t style_get = {
	.uri	  = "/style.css",
	.method   = HTTP_GET,
	.handler  = style_get_handler,
	.user_ctx = NULL
};

httpd_uri_t index_get = {
	.uri	  = "/",
	.method   = HTTP_GET,
	.handler  = index_get_handler,
	.user_ctx = NULL
};

httpd_uri_t about_get = {
	.uri	  = "/about.html",
	.method   = HTTP_GET,
	.handler  = about_get_handler,
	.user_ctx = NULL
};

httpd_uri_t frequencymeter_get = {
	.uri	  = "/frequencymeter.html",
	.method   = HTTP_GET,
	.handler  = frequencymeter_get_handler,
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

httpd_uri_t upload_get = {
	.uri	  = "/upload.html",
	.method   = HTTP_GET,
	.handler  = upload_get_handler,
	.user_ctx = NULL
};

httpd_uri_t wifi_get = {
	.uri	  = "/wifi.html",
	.method   = HTTP_GET,
	.handler  = wifi_get_handler,
	.user_ctx = NULL
};

httpd_uri_t update_post = {
	.uri	  = "/update",
	.method   = HTTP_POST,
	.handler  = update_post_handler,
	.user_ctx = NULL
};

httpd_uri_t reboot_post = {
	.uri	  = "/reboot",
	.method   = HTTP_POST,
	.handler  = reboot_post_handler,
	.user_ctx = NULL
};

httpd_uri_t ws = {
        .uri        = "/ws",
        .method     = HTTP_GET,
        .handler    = ws_handler,
        .user_ctx   = NULL,
        .is_websocket = true,
        .handle_ws_control_frames = true
};

// HTTP server URI handler structure
httpd_uri_t set_wifi_uri_handler = {
    .uri       = "/set_wifi",
    .method    = HTTP_POST,
    .handler   = set_wifi_post_handler,
    .user_ctx  = NULL
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
		httpd_register_uri_handler(http_server, &wifi_get);
		httpd_register_uri_handler(http_server, &update_post);
		httpd_register_uri_handler(http_server, &reboot_post);
		httpd_register_uri_handler(http_server, &ws);
		httpd_register_uri_handler(http_server, &set_wifi_uri_handler);
		wss_keep_alive_set_user_ctx(keep_alive, http_server);

	}
	ws_server_send_data(&http_server);

	return http_server == NULL ? ESP_FAIL : ESP_OK;
}
