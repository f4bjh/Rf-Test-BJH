/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */
#include <esp_ota_ops.h>
#include "esp_log.h"
#include "esp_netif.h"

#include "lvgl.h"

static const char* TAG = "lcd";

void example_lvgl_demo_ui(lv_disp_t *disp)
{
    lv_obj_t *scr = lv_disp_get_scr_act(disp);
    lv_obj_t *label = lv_label_create(scr);
    esp_err_t ret;
    esp_app_desc_t app_desc;
    esp_netif_ip_info_t ip_info;
    esp_netif_t* netif = NULL;

    //for Wifi station
    //netif = esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");    
    //fir Wifi AP
    netif = esp_netif_get_handle_from_ifkey("WIFI_AP_DEF");
    esp_netif_get_ip_info(netif,&ip_info);
    ESP_LOGI(TAG, "ip : "IPSTR ,IP2STR(&ip_info.ip));

    const esp_partition_t *partition = esp_ota_get_running_partition();
    ret = esp_ota_get_partition_description(partition, &app_desc);
    ESP_ERROR_CHECK(ret);

    lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR); /* Circular scroll */
    lv_label_set_text_fmt(label, "%s\n%s\n" IPSTR ,app_desc.project_name, app_desc.version,IP2STR(&ip_info.ip));
    /* Size of the screen (if you use rotation 90 or 270, please set disp->driver->ver_res) */
    lv_obj_set_width(label, disp->driver->hor_res);
    lv_obj_align(label, LV_ALIGN_TOP_LEFT, 0, 0);

}
