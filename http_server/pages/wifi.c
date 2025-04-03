#include "http_server.h"

static const char* TAG = "wifi_uri_handler";

extern const uint8_t wifi_html_start[] asm("_binary_wifi_html_start");
extern const uint8_t wifi_html_end[] asm("_binary_wifi_html_end");

esp_err_t wifi_get_handler(httpd_req_t *req)
{
	httpd_resp_send(req, (const char *) wifi_html_start, wifi_html_end - wifi_html_start);
	return ESP_OK;
}

httpd_uri_t wifi_get = {
	.uri	  = "/wifi.html",
	.method   = HTTP_GET,
	.handler  = wifi_get_handler,
	.user_ctx = NULL
};

// HTTP request handler for setting Wi-Fi credentials
static esp_err_t set_wifi_post_handler(httpd_req_t *req)
{

    char buf[128];
    int ret;
    char param[EXAMPLE_HTTP_QUERY_KEY_MAX_LEN] = {0};
    char ssid[32]= {0};
    char password[64] = {0};
    uint8_t wifi_credentials_set = WIFI_CREDENTIAL_NOT_SET_IN_FLASH;
    size_t buf_len = req->content_len;
    char *ptr;

	if (buf_len>128) {
            httpd_resp_send(req, "Invalid Wi-Fi credentials", HTTPD_RESP_USE_STRLEN);
	    return ESP_FAIL;
	}

    // Read the data for the request
    if ((ret = httpd_req_recv(req, buf, buf_len)) <= 0) {
        if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
            httpd_resp_send_408(req);
        }
        return ESP_FAIL;
    }

    // Process the received data

	ptr = malloc((buf_len+1) * sizeof(char));
	strncpy(ptr,buf,buf_len);
	ptr[buf_len]='\0';

	httpd_query_key_value(ptr, "ssid", param, 32); 
    example_uri_decode(ssid, param, strnlen(param, EXAMPLE_HTTP_QUERY_KEY_MAX_LEN));

	httpd_query_key_value(ptr, "password", param, 64); 
    example_uri_decode(password, param, strnlen(param, EXAMPLE_HTTP_QUERY_KEY_MAX_LEN));
	
	free(ptr);

 	if (strlen(ssid)>32) {
            httpd_resp_send(req, "Invalid ssid", HTTPD_RESP_USE_STRLEN);
	    return ESP_FAIL;
	}
	if (strlen(password)>64) {
            httpd_resp_send(req, "Invalid password", HTTPD_RESP_USE_STRLEN);
	    return ESP_FAIL;
	}


	// Set Wi-Fi credentials
        wifi_credentials_set = WIFI_CREDENTIAL_SET_IN_FLASH;
        nvs_handle_t my_handle;
        esp_err_t err = nvs_open("storage", NVS_READWRITE, &my_handle);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Error (%s) opening NVS handle!\n", esp_err_to_name(err));
        } else {
            err = nvs_set_str(my_handle, NVS_KEY_SSID, ssid);
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "Error (%s) writing ssid\n", esp_err_to_name(err));
            }
            err = nvs_set_str(my_handle, NVS_KEY_PASSWORD, password);
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "Error (%s) writing password\n", esp_err_to_name(err));
            }
	        err = nvs_set_u8(my_handle, NVS_KEY_WIFI_SET_CREDENTIAL, wifi_credentials_set);
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "Error (%s) writing wifi_credentials_set\n", esp_err_to_name(err));
            }

            nvs_close(my_handle);

        }
       
       	if (err != ESP_OK) 
	      httpd_resp_send(req, "Wi-Fi credentials set unsuccessfully in flash", HTTPD_RESP_USE_STRLEN);
	    else
	      httpd_resp_send(req, "Wi-Fi credentials set successfully", HTTPD_RESP_USE_STRLEN);

        return err; 
}

// HTTP server URI handler structure
httpd_uri_t set_wifi_uri_handler = {
    .uri       = "/set_wifi",
    .method    = HTTP_POST,
    .handler   = set_wifi_post_handler,
    .user_ctx  = NULL
};

esp_err_t reboot_post_handler(httpd_req_t *req)
 {
     ESP_LOGI(TAG,"Reboot requested !!!");
     esp_restart();
 
     return ESP_OK;
}
httpd_uri_t reboot_post = {
	.uri	  = "/reboot",
	.method   = HTTP_POST,
	.handler  = reboot_post_handler,
	.user_ctx = NULL
};

