#include "http_server.h"

/*
 * HTTP Server
 */
static const char* TAG = "http_server";

static const size_t max_clients = 4;

extern const uint8_t style_css_start[] asm("_binary_style_css_start");
extern const uint8_t style_css_end[] asm("_binary_style_css_end");
extern httpd_uri_t index_get;
extern httpd_uri_t index2_get;
#if 0
extern const uint8_t index_html_start[] asm("_binary_index_html_start");
extern const uint8_t index_html_end[] asm("_binary_index_html_end");
#endif
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
extern httpd_uri_t ws ;

extern TaskHandle_t xHandle_keep_alive;

TaskHandle_t xHandle_ws_send_msg;

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
#if 0
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
#endif 
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

// Get all clients and send async message
void server_send_data_tsk(void* arg)
{
    bool send_messages = true;
    httpd_handle_t* server=arg;
    server_ctx_t *server_ctx = NULL;
    instance_meas_t  *instance_meas=NULL;
    static struct async_resp_arg *resp_arg;
    
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

          	  resp_arg = malloc(sizeof(struct async_resp_arg));
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
	config.open_fn = ws_open_fd;
	config.close_fn = ws_close_fd;
	config.task_priority = tskHTTP_SERVER;

	if (httpd_start(&http_server, &config) == ESP_OK) {
		httpd_register_uri_handler(http_server, &style_get);
		httpd_register_uri_handler(http_server, &index_get);
		httpd_register_uri_handler(http_server, &index2_get);
		httpd_register_uri_handler(http_server, &about_get);
		httpd_register_uri_handler(http_server, &frequencymeter_get);
		httpd_register_uri_handler(http_server, &generator_get);
		httpd_register_uri_handler(http_server, &powermeter_get);
		httpd_register_uri_handler(http_server, &script_js_get);
		httpd_register_uri_handler(http_server, &upload_get);
        httpd_register_uri_handler(http_server, &update_post);
		httpd_register_uri_handler(http_server, &reboot_after_upload_post);
		httpd_register_uri_handler(http_server, &wifi_get);
        httpd_register_uri_handler(http_server, &reboot_post);
        httpd_register_uri_handler(http_server, &set_wifi_uri_handler);
		httpd_register_uri_handler(http_server, &ws);
		httpd_register_uri_handler(http_server, &jquery_gauge_css_get);
		httpd_register_uri_handler(http_server, &jquery_gauge_js_get);
		httpd_register_uri_handler(http_server, &jquery_gauge_min_js_get);
		httpd_register_uri_handler(http_server, &jquery_min_js_get);
		httpd_register_uri_handler(http_server, &style_get);
        wss_keep_alive_set_user_ctx(keep_alive, http_server);
	}
	//ws_server_send_data(&http_server);
	  xTaskCreatePinnedToCore(server_send_data_tsk, 
		   "ws server send data", 
		   configMINIMAL_STACK_SIZE, 
		   &http_server,
		   tskHTTP_SERVER,
		   &xHandle_ws_send_msg,
		   0);

	return http_server == NULL ? ESP_FAIL : ESP_OK;
}
