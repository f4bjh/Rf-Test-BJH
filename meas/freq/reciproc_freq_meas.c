#include <freertos/FreeRTOS.h>
#include <esp_http_server.h>
#include <freertos/task.h>
#include <esp_ota_ops.h>
#include "esp_log.h"
#include "esp_chip_info.h"
#include <rom/ets_sys.h>
#include <inttypes.h>
#include <driver/spi_master.h>
#include <driver/gpio.h>

#include "meas_mgt.h"
#include "meas.h"

static char TAG[] = "reciproc_freq";

spi_device_handle_t spi_frequencymeter_handle;

void reciproc_freq_write(reciproc_freq_dev *dev, uint32_t reg)
{
    esp_err_t err;

#if 0
    // ensure LE pin is low
    gpio_set_level(dev->pins.gpio_le, 0);
    ets_delay_us(5);
#endif

    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.tx_data[0] = (reg>>24)&0xff;
    t.tx_data[1] = (reg>>16)&0xff;
    t.tx_data[2] = (reg>> 8)&0xff;
    t.tx_data[3] = (reg    )&0xff;
    t.flags = SPI_TRANS_USE_TXDATA;
    t.length = 32;
    t.rx_buffer = NULL;

    err = spi_device_transmit(spi_frequencymeter_handle, &t);
    if (err != ESP_OK)
      ESP_LOGE(TAG,"spi device transmit error %d", err);

#if 0
    gpio_set_level(dev->pins.gpio_le, 1);    // pull LE pin high to latch incoming word into register
    ets_delay_us(5);
    gpio_set_level(dev->pins.gpio_le, 0);    // pull LE pin to its default low
#endif

}

void reciproc_freq_enable(reciproc_freq_cfg_t *pcfg)
{
    // check if CE pin was initialised
    if(pcfg->_ce_initialised)
    {
        gpio_set_level(pcfg->pins.gpio_ce, 1);
        pcfg->_enabled = true;
    }
    else
        ESP_LOGE(TAG, "Attempting to toggle CE pin without initialisation");
}

void reciproc_freq_disable(reciproc_freq_cfg_t *pcfg)
{
    // check if CE pin was initialised
    if(pcfg->_ce_initialised)
    {
        gpio_set_level(pcfg->pins.gpio_ce, 0);
        pcfg->_enabled = false;
    }
    else
        ESP_LOGE(TAG, "Attempting to toggle CE pin without initialisation");
}

int32_t reciproc_freq_TEST_WRITE_SPI_FPGA(reciproc_freq_cfg_t *pcfg)
{
	int32_t ret=0;

	reciproc_freq_enable(pcfg);
	reciproc_freq_write(pcfg->reciproc_freq_device,0x0000FDEC);
	reciproc_freq_disable(pcfg);

	return ret;

}

int32_t reciproc_freq_setup(reciproc_freq_dev **device,
		            reciproc_freq_init_param init_param)
{
	reciproc_freq_dev *dev;
	int32_t ret=0;

	dev = (reciproc_freq_dev *)malloc(sizeof(*dev));
	if (!dev) {
		return -1;
	}

	dev->pdata = (struct reciproc_freq_platform_data *)malloc(sizeof(*dev->pdata));
	if (!dev->pdata)
		return -1;

	dev->pdata->window_time_ms = init_param.window_time_ms;
	dev->pdata->ref_fast_clk_MHz = init_param.ref_fast_clk_MHz;
	dev->pdata->square_out_freq = init_param.square_out_freq;

	*device = dev;

	ESP_LOGI(TAG,"FPGA reciprocal frequency meas successfully initialized.\n");

	return ret;



}


void reciproc_freq_initialise(reciproc_freq_cfg_t *pcfg)
{
    // initialise internal fields
    pcfg->_enabled = false;

#if 0
	// Step1, initalise SPI perpheral and GPIO
	// Configuration for the SPI bus
	spi_bus_config_t buscfg = 
    {
		.mosi_io_num = pcfg->pins.gpio_mosi,
		//.data0_io_num = -1,
		.miso_io_num = -1,
		.data1_io_num = -1,
		.sclk_io_num = pcfg->pins.gpio_sclk,
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

	// Configuration for the SPI device on the other side of the bus
    spi_device_interface_config_t devcfg = 
    {
        .command_bits = 0,
        .address_bits = 0,
        .dummy_bits = 0,
        .clock_speed_hz = APB_CLK_FREQ/80, // run at 1MHz
        .duty_cycle_pos = 128,        // 50% duty cycle
        .mode = 0,
        .spics_io_num = pcfg->pins.gpio_cs, // does not matter, not using the SPI CS pin anyways
        .cs_ena_posttrans = 0,        
        .queue_size = 1
    };

#if 0
    // GPIO settings for load enable pin (active high)
    gpio_config_t reciproc_freq_le_cfg = 
    {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pull_down_en = 1,
        .pin_bit_mask = (1 << pcfg->pins.gpio_le)
    };
#endif

    // GPIO settings for chip enable pin (active high)
    gpio_config_t reciproc_freq_ce_cfg = 
    {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pull_down_en = 1,
        .pin_bit_mask = (1 << pcfg->pins.gpio_ce)
    };

#if 0
    // GPIO settings for lock detect pin (high - PLL lock, low - loss of PLL lock)
    gpio_config_t reciproc_freq_ld_cfg = 
    {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = (1 << pcfg->pins.gpio_ld)
    };
#endif

	esp_err_t ret;

#if 0
    ret = gpio_config(&reciproc_freq_le_cfg);
    assert(ret == ESP_OK);
    pcfg->_le_initialised = true;
#endif

    ret = gpio_config(&reciproc_freq_ce_cfg);
    assert(ret == ESP_OK);
    pcfg->_ce_initialised = true;

#if 0
    ret = gpio_config(&reciproc_freq_ld_cfg);
    assert(ret == ESP_OK);
    pcfg->_ld_initialised = true;
#endif

    // drive LE and CE low just in case
#if 0
    gpio_set_level(pcfg->pins.gpio_le, 0);
#endif
    gpio_set_level(pcfg->pins.gpio_ce, 0);

    ESP_LOGI(TAG, "GPIO successfully initialised");

#if 0
    ESP_LOGI(TAG, "SPI bus initialisation Mode: %d, Clock speed: %d, MOSI GPIO: %d", devcfg.mode, devcfg.clock_speed_hz, buscfg.mosi_io_num);
    ret = spi_bus_initialize(SENDER_HOST, &buscfg, SPI_DMA_DISABLED);
    assert(ret == ESP_OK);

    ESP_LOGI(TAG, "SPI bus successfully initialised");
#endif
    
    ESP_LOGI(TAG, "Attach SPI Reciprocal frequency meas Mode: %d, Clock speed: %d", devcfg.mode, devcfg.clock_speed_hz);
    ret = spi_bus_add_device(SENDER_HOST, &devcfg, &spi_frequencymeter_handle);
    assert(ret == ESP_OK);

    ESP_LOGI(TAG, "SPI device successfully attached");
    pcfg->_spi_initialised = true;
}

