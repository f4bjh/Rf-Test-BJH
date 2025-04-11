#include "http_server.h"

extern TaskHandle_t xHandle_keep_alive;

#ifdef CONFIG_FIRMWARE_OTA
static const char* TAG = "upload_uri_handler";
#endif
#ifdef CONFIG_FIRMWARE_FACTORY
static const char* TAG = "factory_uri_handler";
#endif


#ifdef CONFIG_FIRMWARE_OTA
extern const uint8_t upload_html_start[] asm("_binary_upload_html_start");
extern const uint8_t upload_html_end[] asm("_binary_upload_html_end");

esp_err_t upload_get_handler(httpd_req_t *req)
{
	httpd_resp_send(req, (const char *) upload_html_start, upload_html_end - upload_html_start);
	return ESP_OK;
}
httpd_uri_t upload_get = {
	.uri	  = "/upload.html",
	.method   = HTTP_GET,
	.handler  = upload_get_handler,
	.user_ctx = NULL
};
#endif
#ifdef CONFIG_FIRMWARE_FACTORY
extern const uint8_t factory_html_start[] asm("_binary_factory_html_start");
extern const uint8_t factory_html_end[] asm("_binary_factory_html_end");
esp_err_t upload_get_handler(httpd_req_t *req)
{
	httpd_resp_send(req, (const char *) factory_html_start, factory_html_end - factory_html_start);
	return ESP_OK;
}
httpd_uri_t upload_get = {
	.uri	  = "/index.html",
	.method   = HTTP_GET,
	.handler  = upload_get_handler,
	.user_ctx = NULL
};
httpd_uri_t upload2_get = {
	.uri	  = "/",
	.method   = HTTP_GET,
	.handler  = upload_get_handler,
	.user_ctx = NULL
};
#endif


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
 
     ESP_LOGI(TAG,"Firmware update complete on %s", ota_partition->label);
     vTaskResume (xHandle_keep_alive);

     if (esp_ota_set_boot_partition(ota_partition) != ESP_OK) {
         httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to set boot partition to next");
         return ESP_FAIL;
     }
 
     ESP_LOGI(TAG,"Reboot requested to %s!!!", ota_partition->label);
     httpd_resp_send(req, "Firmware update complete - Rebooting...", HTTPD_RESP_USE_STRLEN);

     esp_restart();
      
     return ESP_OK;
 }
 httpd_uri_t update_post = {
	.uri	  = "/update",
	.method   = HTTP_POST,
	.handler  = update_post_handler,
	.user_ctx = NULL
};

#if 0
 esp_err_t reboot_after_upload_post_handler(httpd_req_t *req)
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

 httpd_uri_t reboot_after_upload_post = {
	.uri	  = "/reboot_after_upload",
	.method   = HTTP_POST,
	.handler  = reboot_after_upload_post_handler,
	.user_ctx = NULL
};
#endif
#ifdef CONFIG_FIRMWARE_OTA
extern const uint8_t upload_js_start[] asm("_binary_upload_js_start");
extern const uint8_t upload_js_end[] asm("_binary_upload_js_end");
esp_err_t upload_js_get_handler(httpd_req_t *req)
{
	httpd_resp_set_type(req, "text/javascript");
	httpd_resp_send(req, (const char *) upload_js_start, upload_js_end - upload_js_start);
	return ESP_OK;
}
httpd_uri_t upload_js_get = {
	.uri	  = "/upload.js",
	.method   = HTTP_GET,
	.handler  = upload_js_get_handler,
	.user_ctx = NULL
};
#endif
