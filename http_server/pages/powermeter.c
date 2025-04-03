#include "http_server.h"

static const char* TAG = "powermeter_uri_handler";

extern const uint8_t powermeter_html_start[] asm("_binary_powermeter_html_start");
extern const uint8_t powermeter_html_end[] asm("_binary_powermeter_html_end");

esp_err_t powermeter_get_handler(httpd_req_t *req)
{
	httpd_resp_send(req, (const char *) powermeter_html_start, powermeter_html_end - powermeter_html_start);
	return ESP_OK;
}

httpd_uri_t powermeter_get = {
	.uri	  = "/powermeter.html",
	.method   = HTTP_GET,
	.handler  = powermeter_get_handler,
	.user_ctx = NULL
};