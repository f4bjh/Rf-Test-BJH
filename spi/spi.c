#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>
#include "esp_log.h"

#include "gpio.h"
#include "spi.h"

static char TAG[] = "spi";


void spi_chip_enable(pin_settings *pins)
{
   pin_conf_t gpio_ce = pins->gpio_ce;	

   if (gpio_ce.gpio_number &&
       (gpio_ce.polarity == ACTIVE_LOW ||
        gpio_ce.polarity == ACTIVE_HIGH))
     gpio_set_level(gpio_ce.gpio_number, gpio_ce.polarity);
   else
     ESP_LOGE(TAG,"cannot set %d on gpio number=%d",gpio_ce.polarity, gpio_ce.gpio_number);
};

void spi_chip_disable(pin_settings *pins)
{
   pin_conf_t gpio_ce = pins->gpio_ce;	

   if (gpio_ce.gpio_number &&
       (gpio_ce.polarity == ACTIVE_LOW ||
        gpio_ce.polarity == ACTIVE_HIGH))
     gpio_set_level(gpio_ce.gpio_number, (gpio_ce.polarity) ^ (0x01));
   else
     ESP_LOGE(TAG,"cannot set %d on gpio number=%d",(gpio_ce.polarity) ^ (0x01), gpio_ce.gpio_number);
};

void spi_chip_reset(pin_settings *pins)
{
   pin_conf_t gpio_por = pins->gpio_por;	

   if (!(gpio_por.gpio_number &&
        (gpio_por.polarity == ACTIVE_LOW ||
         gpio_por.polarity == ACTIVE_HIGH)) ) {
       ESP_LOGE(TAG,"cannot set %d on gpio number=%d",(gpio_por.polarity) ^ (0x01), gpio_por.gpio_number);
       return;
   }
   //reset
   gpio_set_level(gpio_por.gpio_number, (gpio_por.polarity));
   vTaskDelay(1 / portTICK_PERIOD_MS);
   gpio_set_level(gpio_por.gpio_number, (gpio_por.polarity) ^ (0x01));
};
