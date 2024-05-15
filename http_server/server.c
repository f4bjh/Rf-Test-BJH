#include <string.h>

#include <freertos/FreeRTOS.h>
#include <esp_http_server.h>
#include <freertos/task.h>
#include <esp_ota_ops.h>
#include <sys/param.h>
#include "esp_log.h"

#include "http_server.h"


/*
 * HTTP Server
 */
static const char* TAG = "http_server";

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

	// Validate and switch to new OTA image and reboot
	if (esp_ota_end(ota_handle) != ESP_OK || esp_ota_set_boot_partition(ota_partition) != ESP_OK) {
			httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Validation / Activation Error");
			return ESP_FAIL;
	}

	httpd_resp_sendstr(req, "Firmware update complete, rebooting now!\n");

	vTaskDelay(500 / portTICK_PERIOD_MS);
	esp_restart();

	return ESP_OK;
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

esp_err_t http_server_init(void)
{
	static httpd_handle_t http_server = NULL;

	httpd_config_t config = HTTPD_DEFAULT_CONFIG();

	//number of files(html, css, and js) to handle in the http server
	//fix to 15 to have some margin
	config.max_uri_handlers = 15;

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
	}

	return http_server == NULL ? ESP_FAIL : ESP_OK;
}
