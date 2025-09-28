#include <driver/spi_master.h>

#define SENDER_HOST SPI3_HOST

typedef struct 
{
    uint8_t gpio_cs; // dummy pin
    uint8_t gpio_ce; // chip enable
    uint8_t gpio_le; // load enable
    uint8_t gpio_ld; // lock detect
} pin_settings;

