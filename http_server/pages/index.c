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
