#define WIFI_CREDENTIAL_SET_IN_FLASH 0
#define WIFI_CREDENTIAL_NOT_SET_IN_FLASH 1

#define NVS_KEY_SSID "wifissid"
#define NVS_KEY_PASSWORD "wifipassword"
#define NVS_KEY_WIFI_SET_CREDENTIAL "wifiset"

/* AP Configuration */
#define EXAMPLE_ESP_WIFI_AP_SSID            "Rf-Test-BJH AP"
//#define EXAMPLE_ESP_WIFI_AP_PASSWD          rftestbjh
#define EXAMPLE_ESP_WIFI_AP_PASSWD          ""

/* STA Configuration */
#define EXAMPLE_ESP_WIFI_STA_SSID           "your_ssid"
#define EXAMPLE_ESP_WIFI_STA_PASSWD         "your_password"

#define WIFI_MODE_AP_BIT    0
#define WIFI_MODE_STA_BIT   1

void wifi_init(void);
