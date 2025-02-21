/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */
#include <esp_ota_ops.h>
#include "esp_log.h"
#include "esp_netif.h"

#include "wifi.h"
#include "lvgl.h"

extern uint8_t  wifi_mode;

static const char* TAG = "lcd";

void example_lvgl_demo_ui(lv_disp_t *disp)
{
    lv_obj_t *scr = lv_disp_get_scr_act(disp);
    lv_obj_t *label = lv_label_create(scr);
    esp_err_t ret;
    esp_app_desc_t app_desc;
    esp_netif_ip_info_t ip_info_ap, ip_info_sta;
    esp_netif_t* netif_ap, *netif_sta = NULL;

    //for Wifi AP mode
    if (wifi_mode & (1<<WIFI_MODE_AP_BIT)) {
      netif_ap = esp_netif_get_handle_from_ifkey("WIFI_AP_DEF");  
      esp_netif_get_ip_info(netif_ap,&ip_info_ap);
      ESP_LOGI(TAG, "AP ip : "IPSTR ,IP2STR(&ip_info_ap.ip));  
    }
    //for Wifi station mode
    if (wifi_mode & (1<<WIFI_MODE_STA_BIT)) {
      netif_sta = esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");  
      esp_netif_get_ip_info(netif_sta,&ip_info_sta);
      ESP_LOGI(TAG, "STA ip : "IPSTR ,IP2STR(&ip_info_sta.ip));  
    }


    const esp_partition_t *partition = esp_ota_get_running_partition();
    ret = esp_ota_get_partition_description(partition, &app_desc);
    ESP_ERROR_CHECK(ret);

    lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR); /* Circular scroll */

    if (wifi_mode & (1<<WIFI_MODE_AP_BIT)) {
      lv_label_set_text_fmt(label, "%s\n%s\n" IPSTR ,app_desc.project_name, app_desc.version,IP2STR(&ip_info_ap.ip));
    } else if (wifi_mode & (1<<WIFI_MODE_STA_BIT)) {
       lv_label_set_text_fmt(label, "%s\n%s\n" IPSTR ,app_desc.project_name, app_desc.version,IP2STR(&ip_info_sta.ip));
    } else {
      lv_label_set_text_fmt(label, "%s\n%s\n IP address error",app_desc.project_name, app_desc.version);
    }
    /* Size of the screen (if you use rotation 90 or 270, please set disp->driver->ver_res) */
    lv_obj_set_width(label, disp->driver->hor_res);
    lv_obj_align(label, LV_ALIGN_TOP_LEFT, 0, 0);

}
