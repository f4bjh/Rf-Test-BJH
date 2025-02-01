
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include <esp_ota_ops.h>
#include <nvs_flash.h>
#include "esp_log.h"

#include "http_server.h"
#include "softap.h"
#include "data.h"

static const char* TAG = "main";

void app_main(void) {
	esp_err_t ret = nvs_flash_init();

	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK(ret);

	const esp_partition_t *partition = esp_ota_get_running_partition();
	ESP_LOGI(TAG, "Currently running partition: %s", partition->label);

	ESP_ERROR_CHECK(data_init());
	ESP_ERROR_CHECK(softap_init());
	ESP_ERROR_CHECK(http_server_init());

	esp_ota_img_states_t ota_state;
	if (esp_ota_get_state_partition(partition, &ota_state) == ESP_OK) {
		if (ota_state == ESP_OTA_IMG_PENDING_VERIFY) {
			esp_ota_mark_app_valid_cancel_rollback();
		}
	}

	while(1) vTaskDelay(10);
}

