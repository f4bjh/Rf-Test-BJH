/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_err.h"
#include "esp_log.h"
#include "driver/i2c_master.h"
#include "esp_lvgl_port.h"
#include "lvgl.h"
#include "lcd.h"
#include "gpio.h"
#include "esp_netif.h"

#include "wifi.h"

#if CONFIG_EXAMPLE_LCD_CONTROLLER_SH1107
#include "esp_lcd_sh1107.h"
#else
#include "esp_lcd_panel_vendor.h"
#endif

static const char *TAG = "lcd";
extern uint8_t  wifi_mode;
lv_disp_t *disp;

#define I2C_BUS_PORT  0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////// Please update the following configuration according to your LCD spec //////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define EXAMPLE_LCD_PIXEL_CLOCK_HZ    (400 * 1000)
#define EXAMPLE_PIN_NUM_RST           -1
#define EXAMPLE_I2C_HW_ADDR           0x3C

// The pixel number in horizontal and vertical
#if CONFIG_EXAMPLE_LCD_CONTROLLER_SSD1306
#define EXAMPLE_LCD_H_RES              128
#define EXAMPLE_LCD_V_RES              CONFIG_EXAMPLE_SSD1306_HEIGHT
#elif CONFIG_EXAMPLE_LCD_CONTROLLER_SH1107
#define EXAMPLE_LCD_H_RES              64
#define EXAMPLE_LCD_V_RES              128
#endif
// Bit number used to represent command and parameter
#define EXAMPLE_LCD_CMD_BITS           8
#define EXAMPLE_LCD_PARAM_BITS         8

void lcd_clear_screen()
{

  lv_obj_clean(lv_scr_act());
  lv_obj_set_style_bg_color(lv_scr_act(), lv_color_black(), LV_PART_MAIN);
  lv_refr_now(NULL);

}

void lcd_display_at_boot()
{
    lv_obj_t *scr = lv_disp_get_scr_act(disp);
    lv_obj_t *label = lv_label_create(scr);
    esp_netif_ip_info_t ip_info_ap, ip_info_sta;
    esp_netif_t* netif_ap, *netif_sta = NULL;

    //for Wifi AP mode
    if (wifi_mode & (1<<WIFI_MODE_AP_BIT)) {
      netif_ap = esp_netif_get_handle_from_ifkey("WIFI_AP_DEF");  
      esp_netif_get_ip_info(netif_ap,&ip_info_ap);
    } else 
      ip_info_ap.ip.addr = 0;
    
    ESP_LOGI(TAG, "AP ip : "IPSTR ,IP2STR(&ip_info_ap.ip));  

    ip_info_sta.ip.addr = 0;

#ifdef CONFIG_FIRMWARE_OTA
    //for Wifi station mode
    if (wifi_mode & (1<<WIFI_MODE_STA_BIT)) {
      netif_sta = esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");  
      esp_netif_get_ip_info(netif_sta,&ip_info_sta);
    }
#endif

    ESP_LOGI(TAG, "STA ip : "IPSTR ,IP2STR(&ip_info_sta.ip));  
 

#if 0
    const esp_partition_t *partition = esp_ota_get_running_partition();
    ret = esp_ota_get_partition_description(partition, &app_desc);
    ESP_ERROR_CHECK(ret);
#endif

    lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR); /* Circular scroll */
    
    if (wifi_mode & (1<<WIFI_MODE_AP_BIT)) {
    	lv_label_set_text_fmt(label, "Rf-Test-BJH AP\n"IPSTR , IP2STR(&ip_info_ap.ip));
    }

#ifdef CONFIG_FIRMWARE_OTA
    //for Wifi station mode
    if (wifi_mode & (1<<WIFI_MODE_STA_BIT)) {
	lv_label_set_text_fmt(label, "Rf-Test-BJH STA\n"IPSTR ,IP2STR(&ip_info_sta.ip));
    }
#endif
 
    /* Size of the screen (if you use rotation 90 or 270, please set disp->driver->ver_res) */
    lv_obj_set_width(label, disp->driver->hor_res);
    lv_obj_align(label, LV_ALIGN_TOP_LEFT, 0, 0);

}

void lcd_display(void)
{

    // Lock the mutex due to the LVGL APIs are not thread-safe
    if (lvgl_port_lock(0)) {
        lcd_display_at_boot(disp);
        // Release the mutex
        lvgl_port_unlock();
    }
}

esp_err_t lcd_init(void)
{
    esp_err_t ret;

    ESP_LOGI(TAG, "Initialize I2C bus");
    i2c_master_bus_handle_t i2c_bus = NULL;
    i2c_master_bus_config_t bus_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .i2c_port = I2C_BUS_PORT,
        .sda_io_num = GPIO_LCD_SDA,
        .scl_io_num = GPIO_LCD_SCL,
        .flags.enable_internal_pullup = true,
    };
    ret = i2c_new_master_bus(&bus_config, &i2c_bus);
    if (ret != ESP_OK) {
      ESP_LOGE(TAG, "Failed to initialize I2C bus");
      return ret;
    }


    ESP_LOGI(TAG, "Install panel IO");
    esp_lcd_panel_io_handle_t io_handle = NULL;
    esp_lcd_panel_io_i2c_config_t io_config = {
        .dev_addr = EXAMPLE_I2C_HW_ADDR,
        .scl_speed_hz = EXAMPLE_LCD_PIXEL_CLOCK_HZ,
        .control_phase_bytes = 1,               // According to SSD1306 datasheet
        .lcd_cmd_bits = EXAMPLE_LCD_CMD_BITS,   // According to SSD1306 datasheet
        .lcd_param_bits = EXAMPLE_LCD_CMD_BITS, // According to SSD1306 datasheet
#if CONFIG_EXAMPLE_LCD_CONTROLLER_SSD1306
        .dc_bit_offset = 6,                     // According to SSD1306 datasheet
#elif CONFIG_EXAMPLE_LCD_CONTROLLER_SH1107
        .dc_bit_offset = 0,                     // According to SH1107 datasheet
        .flags =
        {
            .disable_control_phase = 1,
        }
#endif
    };
    ret = esp_lcd_new_panel_io_i2c(i2c_bus, &io_config, &io_handle);
    if (ret != ESP_OK) {
      ESP_LOGE(TAG, "Failed to install panel IO");
      return ret;
    }

    ESP_LOGI(TAG, "Install SSD1306 panel driver");
    esp_lcd_panel_handle_t panel_handle = NULL;
    esp_lcd_panel_dev_config_t panel_config = {
        .bits_per_pixel = 1,
        .reset_gpio_num = EXAMPLE_PIN_NUM_RST,
    };
#if CONFIG_EXAMPLE_LCD_CONTROLLER_SSD1306
    esp_lcd_panel_ssd1306_config_t ssd1306_config = {
        .height = EXAMPLE_LCD_V_RES,
    };
    panel_config.vendor_config = &ssd1306_config;
    ret = esp_lcd_new_panel_ssd1306(io_handle, &panel_config, &panel_handle);
    if (ret != ESP_OK) {
      ESP_LOGE(TAG, "Failed to initialize SSD 1306");
      return ret;
    }
#elif CONFIG_EXAMPLE_LCD_CONTROLLER_SH1107
    ESP_ERROR_CHECK(esp_lcd_new_panel_sh1107(io_handle, &panel_config, &panel_handle));
#endif

    ret= esp_lcd_panel_reset(panel_handle);
    if (ret != ESP_OK) {
      ESP_LOGE(TAG, "Failed to reset panel");
      return ret;
    }

    ret= esp_lcd_panel_init(panel_handle);
    if (ret != ESP_OK) {
      ESP_LOGE(TAG, "Failed to init panel");
      return ret;
    }
    ret=esp_lcd_panel_disp_on_off(panel_handle, true);
    if (ret != ESP_OK) {
      ESP_LOGE(TAG, "Failed to on-off display");
      return ret;
    }

#if CONFIG_EXAMPLE_LCD_CONTROLLER_SH1107
    ESP_ERROR_CHECK(esp_lcd_panel_invert_color(panel_handle, true));
#endif

    ESP_LOGI(TAG, "Initialize LVGL");
    const lvgl_port_cfg_t lvgl_cfg = ESP_LVGL_PORT_INIT_CONFIG();
    lvgl_port_init(&lvgl_cfg);

    const lvgl_port_display_cfg_t disp_cfg = {
        .io_handle = io_handle,
        .panel_handle = panel_handle,
        .buffer_size = EXAMPLE_LCD_H_RES * EXAMPLE_LCD_V_RES,
        .double_buffer = true,
        .hres = EXAMPLE_LCD_H_RES,
        .vres = EXAMPLE_LCD_V_RES,
        .monochrome = true,
        .rotation = {
            .swap_xy = false,
            .mirror_x = false,
            .mirror_y = false,
        }
    };
    disp = lvgl_port_add_disp(&disp_cfg);

    /* Rotation of the screen */
    lv_disp_set_rotation(disp, LV_DISP_ROT_NONE);

    return ESP_OK;
}
