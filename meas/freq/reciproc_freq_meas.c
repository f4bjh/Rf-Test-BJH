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

#include "gpio.h"
#include "spi.h"
#include "reciproc_freq_meas.h"

static char TAG[] = "reciproc_freq";

spi_device_handle_t spi_frequencymeter_handle;

void reciproc_freq_write(reciproc_freq_dev *dev, uint32_t reg)
{
    esp_err_t err;

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

}

void reciproc_freq_enable(reciproc_freq_cfg_t *pcfg)
{
    // check if CE pin was initialised
    if(pcfg->_ce_initialised)
    {
        spi_chip_enable(&(pcfg->pins));
        pcfg->_enabled = true;
    }
    else
        ESP_LOGE(TAG, "Attempting to toggle CE pin without initialisation");
}

void reciproc_freq_reset(reciproc_freq_cfg_t *pcfg)
{
    // check if POR pin was initialised
    if(pcfg->_por_initialised)
    {
        spi_chip_reset(&(pcfg->pins));
    }
    else
        ESP_LOGE(TAG, "Attempting to toggle POR pin without initialisation");
}

void reciproc_freq_disable(reciproc_freq_cfg_t *pcfg)
{
    // check if CE pin was initialised
    if(pcfg->_ce_initialised)
    {
        spi_chip_disable(&(pcfg->pins));
        pcfg->_enabled = false;
    }
    else
        ESP_LOGE(TAG, "Attempting to toggle CE pin without initialisation");
}

void reciproc_freq_send_spi_data(reciproc_freq_cfg_t *pcfg, uint8_t cmd, uint8_t param[])
{

	uint32_t spi_data;

	reciproc_freq_enable(pcfg);
	spi_data = (cmd&0xFF) <<24;
	spi_data |= (param[0]&0xFF)<<16;
	spi_data |= (param[1]&0xFF)<<8;
	spi_data |= (param[2]&0xFF);
	ESP_LOGI(TAG,"send 0x%" PRIX32 " to FPGA",spi_data);
	reciproc_freq_write(pcfg->reciproc_freq_device,spi_data);
	reciproc_freq_disable(pcfg);

}

bool led_state=false;
int32_t reciproc_freq_TEST_TOGGLE_LED(reciproc_freq_cfg_t *pcfg)
{

	int32_t ret=0;
	uint8_t cmd;
	uint8_t param[3];

	led_state =!led_state;
	cmd = (led_state == true ? RECIPROC_FREQ_MEAS_CMD_SPI_LED_ON : RECIPROC_FREQ_MEAS_CMD_SPI_LED_OFF);
	param[0] = 0;
	param[1] = 0;
	param[2] = 0;

	reciproc_freq_send_spi_data(pcfg,cmd,param);

	return ret;

}

uint32_t reciproq_freq_val[]={0,100000,1000000,6000000};
uint8_t Index=0;
int32_t reciproc_freq_TEST_SET_FREQ(reciproc_freq_cfg_t *pcfg)
{

	int32_t ret=0;
	uint8_t cmd;
	uint8_t param[3];

	if (Index == 3)
		Index = 0;
	else
		Index++;

	cmd = RECIPROC_FREQ_MEAS_CMD_SPI_SET_NCO_FREQ;
	param[0] = (uint8_t) (((reciproq_freq_val[Index])&(0xFF0000))>> 16);
	param[1] = (uint8_t) (((reciproq_freq_val[Index])&(0xFF00))>> 8);
	param[2] = (uint8_t) (((reciproq_freq_val[Index])&(0xFF)));
	
	reciproc_freq_send_spi_data(pcfg,cmd,param);

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
    esp_err_t ret;

    // initialise internal fields
    pcfg->_enabled = false;

    // GPIO settings for chip select pin (active low)
    gpio_config_t reciproc_freq_cs_n_cfg = 
    {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pull_down_en = 1,
        .pin_bit_mask = (1 << pcfg->pins.gpio_ce.gpio_number)
    };


    ret = gpio_config(&reciproc_freq_cs_n_cfg);
    assert(ret == ESP_OK);
    pcfg->_ce_initialised = true;

    reciproc_freq_disable(pcfg);
    ESP_LOGI(TAG, "GPIO successfully initialised");

    // Configuration for the SPI device on the other side of the bus
    spi_device_interface_config_t devcfg = 
    {
        .command_bits = 0,
        .address_bits = 0,
        .dummy_bits = 0,
        .clock_speed_hz = APB_CLK_FREQ/80, // run at 1MHz
        .duty_cycle_pos = 128,        // 50% duty cycle
        .mode = 0,
        .spics_io_num = pcfg->pins.gpio_ce.gpio_number, // does not matter, not using the SPI CS pin anyways
        .cs_ena_posttrans = 0,        
        .queue_size = 1
    };
    ESP_LOGI(TAG, "Attach SPI Reciprocal frequency meas Mode: %d, Clock speed: %d", devcfg.mode, devcfg.clock_speed_hz);
    ret = spi_bus_add_device(SENDER_HOST, &devcfg, &spi_frequencymeter_handle);
    assert(ret == ESP_OK);
    ESP_LOGI(TAG, "SPI device successfully attached");

    // GPIO settings for reset pin (active low)
    gpio_config_t reciproc_freq_por_cfg = 
    {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pull_down_en = 1,
        .pin_bit_mask = (1 << pcfg->pins.gpio_por.gpio_number)
    };
    ret = gpio_config(&reciproc_freq_por_cfg);
    assert(ret == ESP_OK);
    pcfg->_por_initialised = true;
 
    //reset the chip
    reciproc_freq_reset(pcfg);

    pcfg->_spi_initialised = true;
}

void stop_reciproc_freq_meas(reciproc_freq_cfg_t *pcfg)
{

  reciproc_freq_disable(pcfg);
  //reset the chip
  reciproc_freq_reset(pcfg);

}
