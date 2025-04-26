#include "http_server.h"

static const char* TAG = "generator_uri_handler";


extern const uint8_t generator_html_start[] asm("_binary_generator_html_start");
extern const uint8_t generator_html_end[] asm("_binary_generator_html_end");

esp_err_t generator_get_handler(httpd_req_t *req)
{
	httpd_resp_send(req, (const char *) generator_html_start, generator_html_end - generator_html_start);
	return ESP_OK;
}


httpd_uri_t generator_get = {
	.uri	  = "/generator.html",
	.method   = HTTP_GET,
	.handler  = generator_get_handler,
	.user_ctx = NULL
};
extern const uint8_t generator_js_start[] asm("_binary_generator_js_start");
extern const uint8_t generator_js_end[] asm("_binary_generator_js_end");
esp_err_t generator_js_get_handler(httpd_req_t *req)
{
	httpd_resp_set_type(req, "text/javascript");
	httpd_resp_send(req, (const char *) generator_js_start, generator_js_end - generator_js_start);
	return ESP_OK;
}
httpd_uri_t generator_js_get = {
	.uri	  = "/generator.js",
	.method   = HTTP_GET,
	.handler  = generator_js_get_handler,
	.user_ctx = NULL
};
