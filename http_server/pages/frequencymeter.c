#include "http_server.h"

static const char* TAG = "frequencymeter_uri_handler";

extern const uint8_t frequencymeter_html_start[] asm("_binary_frequencymeter_html_start");
extern const uint8_t frequencymeter_html_end[] asm("_binary_frequencymeter_html_end");
esp_err_t frequencymeter_get_handler(httpd_req_t *req)
{
	httpd_resp_send(req, (const char *) frequencymeter_html_start, frequencymeter_html_end - frequencymeter_html_start);
	return ESP_OK;
}
httpd_uri_t frequencymeter_get = {
	.uri	  = "/frequencymeter.html",
	.method   = HTTP_GET,
	.handler  = frequencymeter_get_handler,
	.user_ctx = NULL
};
extern const uint8_t frequencymeter_js_start[] asm("_binary_frequencymeter_js_start");
extern const uint8_t frequencymeter_js_end[] asm("_binary_frequencymeter_js_end");
esp_err_t frequencymeter_js_get_handler(httpd_req_t *req)
{
	httpd_resp_set_type(req, "text/javascript");
	httpd_resp_send(req, (const char *) frequencymeter_js_start, frequencymeter_js_end - frequencymeter_js_start);
	return ESP_OK;
}
httpd_uri_t frequencymeter_js_get = {
	.uri	  = "/frequencymeter.js",
	.method   = HTTP_GET,
	.handler  = frequencymeter_js_get_handler,
	.user_ctx = NULL
};