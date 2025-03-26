#include "http_server.h"

static const char* TAG = "about_uri_handler";

extern const uint8_t about_html_start[] asm("_binary_about_html_start");
extern const uint8_t about_html_end[] asm("_binary_about_html_end");


esp_err_t about_get_handler(httpd_req_t *req)
{
	httpd_resp_send(req, (const char *) about_html_start, about_html_end - about_html_start);
	return ESP_OK;
}

httpd_uri_t about_get = {
	.uri	  = "/about.html",
	.method   = HTTP_GET,
	.handler  = about_get_handler,
	.user_ctx = NULL
};
