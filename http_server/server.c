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

#include "main.h"
#include "http_server.h"
#include "data.h"

extern QueueHandle_t xQueue;

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

struct async_resp_arg {
    httpd_handle_t hd;
    int fd;
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
        ESP_LOGI(TAG, "Received PONG message (fd=%d) - update the keep-alive",httpd_req_to_sockfd(req));
        free(buf);
	
	return wss_keep_alive_client_is_active(httpd_get_global_user_ctx(req->handle),
                httpd_req_to_sockfd(req));

    // If it was a TEXT message, just echo it back
    } else if (ws_pkt.type == HTTPD_WS_TYPE_TEXT || ws_pkt.type == HTTPD_WS_TYPE_PING || ws_pkt.type == HTTPD_WS_TYPE_CLOSE) {
        if (ws_pkt.type == HTTPD_WS_TYPE_TEXT) {
            ESP_LOGI(TAG, "Received packet with message (fd=%d): %s",httpd_req_to_sockfd(req), ws_pkt.payload);
	} else if (ws_pkt.type == HTTPD_WS_TYPE_PING) {
            // Response PONG packet to peer
            ESP_LOGI(TAG, "Got a WS PING frame, Replying PONG");
            ws_pkt.type = HTTPD_WS_TYPE_PONG;
        } else if (ws_pkt.type == HTTPD_WS_TYPE_CLOSE) {
            // Response CLOSE packet with no payload to peer
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
    char json_string_rcv[JSON_STRING_SIZE_OF_MEASUREMENTS *sizeof(char)];

#ifndef DISABLE_WDT_TASK
    esp_task_wdt_status(NULL);
#endif

    if (xQueue != NULL){

      if(xQueueReceive(xQueue, json_string_rcv , 0 ) == pdTRUE) {
        memset(&ws_pkt, 0, sizeof(httpd_ws_frame_t));
        ws_pkt.payload = (uint8_t*)json_string_rcv;
        ws_pkt.len = strlen(json_string_rcv);
        ws_pkt.type = HTTPD_WS_TYPE_TEXT;
        ESP_LOGI(TAG, "send async (%p) %d bytes of data to ws client (fd=%d)", json_string_rcv, ws_pkt.len, fd);
        ESP_LOGV(TAG, "data to sent from queue (%p) is :",xQueue);
	ESP_LOGV(TAG, "%s",ws_pkt.payload);
        esp_err_t err = httpd_ws_send_frame_async(hd, fd, &ws_pkt);

	if (err != ESP_OK) {
    		ESP_LOGE(TAG, "failed to send WebSocket message: %s", esp_err_to_name(err));
	}

      } else
	ESP_LOGI(TAG, "no data found in queue (%p)", xQueue);
    } else {
      ESP_LOGE(TAG,"xQueue measurement not created\n"); 	    
    }

    assert(resp_arg!=NULL);
    free(resp_arg);

}

// Get all clients and send async message
static void ws_server_send_data(httpd_handle_t* server)
{
    bool send_messages = true;
    //const TickType_t xTicksToWait = pdMS_TO_TICKS( 10000 );
   
#ifndef DISABLE_WDT_TASK
    esp_task_wdt_add(NULL);
#endif

    // Send async message to all connected clients that use websocket protocol every 10 seconds
    while (send_messages) {

	vTaskDelay(HTTP_SERVER_WAKE_UP_TICK / portTICK_PERIOD_MS);

	if (!*server) { // httpd might not have been created by now
	    continue;
	}
	size_t clients = max_clients;
	int    client_fds[max_clients];
	if (httpd_get_client_list(*server, &clients, client_fds) == ESP_OK) {
	    for (size_t i=0; i < clients; ++i) {
		int sock = client_fds[i];
		if (httpd_ws_get_fd_info(*server, sock) == HTTPD_WS_CLIENT_WEBSOCKET) {
		    ESP_LOGV(TAG, "sending async message to active client (fd=%d)", sock);
		    struct async_resp_arg *resp_arg = malloc(sizeof(struct async_resp_arg));
		    assert(resp_arg != NULL);
		    resp_arg->hd = *server;
		    resp_arg->fd = sock;
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
    ESP_LOGI(TAG, "New client connected %d", sockfd);
    wss_keep_alive_t h = httpd_get_global_user_ctx(hd);
    return wss_keep_alive_add_client(h, sockfd);
}

void wss_close_fd(httpd_handle_t hd, int sockfd)
{
    ESP_LOGI(TAG, "Client disconnected %d", sockfd);
    wss_keep_alive_t h = httpd_get_global_user_ctx(hd);
    wss_keep_alive_remove_client(h, sockfd);
    close(sockfd);
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

esp_err_t http_server_init(void)
{
	//static httpd_handle_t http_server = NULL;

	// Prepare keep-alive engine
	wss_keep_alive_config_t keep_alive_config = KEEP_ALIVE_CONFIG_DEFAULT();
	keep_alive_config.max_clients = max_clients;
	keep_alive_config.client_not_alive_cb = client_not_alive_cb;
	keep_alive_config.check_client_alive_cb = check_client_alive_cb;
	wss_keep_alive_t keep_alive = wss_keep_alive_start(&keep_alive_config);

	httpd_config_t config = HTTPD_DEFAULT_CONFIG();

	//number of files(html, css, and js) to handle in the http server
	//fix to 255 to have some margin
	config.max_uri_handlers = 255;

	config.max_open_sockets = max_clients;
	config.global_user_ctx = keep_alive;
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
		wss_keep_alive_set_user_ctx(keep_alive, http_server);

	}
	ws_server_send_data(&http_server);

	return http_server == NULL ? ESP_FAIL : ESP_OK;
}
