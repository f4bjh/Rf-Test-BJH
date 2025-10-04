#include <driver/spi_master.h>

#define SENDER_HOST SPI3_HOST

typedef struct 
{
    uint8_t gpio_number;
    uint8_t polarity;
} pin_conf_t;

typedef struct 
{
    pin_conf_t gpio_ce; // chip enable or chip select
    pin_conf_t gpio_le; // load enable
    pin_conf_t gpio_ld; // lock detect
    pin_conf_t gpio_por; // power on reset
} pin_settings;

void spi_chip_enable(pin_settings *pins);
void spi_chip_disable(pin_settings *pins);
void spi_chip_reset(pin_settings *pins);
