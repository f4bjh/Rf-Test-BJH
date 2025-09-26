#include "main.h"

static const char* TAG = "main";

char ssid[] = EXAMPLE_ESP_WIFI_STA_SSID;
char password[] = EXAMPLE_ESP_WIFI_STA_PASSWD;
bool wifi_credentials_set=false;

void app_main(void) {
#ifdef CONFIG_FIRMWARE_OTA
	spi_bus_config_t buscfg = 
    {
		.mosi_io_num = GPIO_SPI_MOSI,
		//.data0_io_num = -1,
		.miso_io_num = -1, //GPIO_SPI_MISO
		.data1_io_num = -1,
		.sclk_io_num = GPIO_SPI_CLK,
		.quadwp_io_num = -1,
		.data2_io_num = -1,
		.quadhd_io_num = -1,
		.data2_io_num = -1,
		.data3_io_num = -1,
		.data4_io_num = -1,
		.data5_io_num = -1,
		.data6_io_num = -1,
		.data7_io_num = -1,
	};
#endif


	esp_err_t ret = nvs_flash_init();
	esp_err_t ret_lcd;
	esp_app_desc_t app_desc;

	ret_lcd = lcd_init();
#if 0	
	if (ret_lcd == ESP_OK) {
	  lcd_clear_screen();
	  ESP_LOGI(TAG,"LCD initialised");
	}
#endif

	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK(ret);

#ifdef CONFIG_FIRMWARE_FACTORY
        ESP_LOGI(TAG,"Currently running a factory fw");
#endif

#ifdef CONFIG_FIRMWARE_OTA
         ESP_LOGI(TAG,"Currently running an ota fw");
#endif

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
#ifdef CONFIG_FIRMWARE_FACTORY
	wifi_credentials_set  = false;
#endif
#ifdef CONFIG_FIRMWARE_OTA
	//Initialise SPI bus
	ESP_LOGI(TAG, "SPI bus initialisation Mode: %d, Clock speed: %d, MOSI GPIO: %d", devcfg.mode, devcfg.clock_speed_hz, buscfg.mosi_io_num);
    ret = spi_bus_initialize(SENDER_HOST, &buscfg, SPI_DMA_DISABLED);
    assert(ret == ESP_OK);

    ESP_LOGI(TAG, "SPI bus successfully initialised");

	//get wifi credentials
	nvs_handle_t handle;
	ret = nvs_open("storage", NVS_READWRITE, &handle);
	ESP_ERROR_CHECK(ret);

	char wifi_ssid[32];
	size_t wifi_ssid_len = sizeof(wifi_ssid);
	ret = nvs_get_str(handle, NVS_KEY_SSID, wifi_ssid, &wifi_ssid_len);

	char wifi_password[64];
	size_t wifi_password_len = sizeof(wifi_password);
	ret = nvs_get_str(handle, NVS_KEY_PASSWORD, wifi_password, &wifi_password_len);

	uint8_t wifi_credentials_set_u8;
	ret = nvs_get_u8(handle, NVS_KEY_WIFI_SET_CREDENTIAL, &wifi_credentials_set_u8);
	wifi_credentials_set = wifi_credentials_set_u8 == WIFI_CREDENTIAL_SET_IN_FLASH;

	if (wifi_credentials_set) {
	  strcpy((char*)ssid, (const char*)wifi_ssid);
          strcpy((char*)password, (const char*)wifi_password);
  	  ESP_LOGI(TAG,"wifi credentials set");
	} else
	  ESP_LOGI(TAG,"wifi credentials not set");

	nvs_close(handle);
#endif

	wifi_init();
	ESP_LOGI(TAG,"WIFI initialised");

	ESP_ERROR_CHECK(http_server_init());
	ESP_LOGI(TAG,"HTTP server initialised");

	esp_ota_img_states_t ota_state;
	if (esp_ota_get_state_partition(partition, &ota_state) == ESP_OK) {
		if (ota_state == ESP_OTA_IMG_PENDING_VERIFY) {
			esp_ota_mark_app_valid_cancel_rollback();
		}
	}

	if (ret_lcd == ESP_OK) 
	  lcd_display();

	while(1) vTaskDelay(10);
}

