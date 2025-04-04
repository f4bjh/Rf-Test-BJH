#include "http_server.h"

static const char* TAG = "index_uri_handler";

extern const uint8_t index_html_start[] asm("_binary_index_html_start");
extern const uint8_t index_html_end[] asm("_binary_index_html_end");

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

extern const uint8_t index_js_start[] asm("_binary_index_js_start");
extern const uint8_t index_js_end[] asm("_binary_index_js_end");
esp_err_t index_js_get_handler(httpd_req_t *req)
{
	httpd_resp_set_type(req, "text/javascript");
	httpd_resp_send(req, (const char *) index_js_start, index_js_end - index_js_start);
	return ESP_OK;
}
httpd_uri_t index_js_get = {
	.uri	  = "/index.js",
	.method   = HTTP_GET,
	.handler  = index_js_get_handler,
	.user_ctx = NULL
};
