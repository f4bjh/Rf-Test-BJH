
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include <esp_ota_ops.h>
#include <nvs_flash.h>
#include "esp_log.h"

#include "main.h"
#include "http_server.h"
#include "wifi.h"
#include "data.h"
#include "lcd.h"

static const char* TAG = "main";

char ssid[] = EXAMPLE_ESP_WIFI_STA_SSID;
char password[] = EXAMPLE_ESP_WIFI_STA_PASSWD;
bool wifi_credentials_set=true;

void app_main(void) {
	esp_err_t ret = nvs_flash_init();
	esp_app_desc_t app_desc;

	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK(ret);

	const esp_partition_t *partition = esp_ota_get_running_partition();
	ESP_LOGI(TAG, "Currently running partition: %s", partition->label);
	ret = esp_ota_get_partition_description(partition, &app_desc);
	ESP_ERROR_CHECK(ret);
	ESP_LOGI(TAG,"Magic word=%08X",(unsigned int)app_desc.magic_word);     
	ESP_LOGI(TAG,"Secure version=%08X",(unsigned int)app_desc.secure_version);    
	ESP_LOGI(TAG,"Application version=%s",app_desc.version);
	ESP_LOGI(TAG,"Project name=%s",app_desc.project_name);
	ESP_LOGI(TAG,"Compile time=%s",app_desc.time);
	ESP_LOGI(TAG,"Compile date=%s",app_desc.date);
	ESP_LOGI(TAG,"Version IDF=%s",app_desc.idf_ver);

	ESP_ERROR_CHECK(data_init());
	wifi_init();
	lcd_init();

	ESP_ERROR_CHECK(http_server_init());
	
	esp_ota_img_states_t ota_state;
	if (esp_ota_get_state_partition(partition, &ota_state) == ESP_OK) {
		if (ota_state == ESP_OTA_IMG_PENDING_VERIFY) {
			esp_ota_mark_app_valid_cancel_rollback();
		}
	}

	while(1) vTaskDelay(10);
}

