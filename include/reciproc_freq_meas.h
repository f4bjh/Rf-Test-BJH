//
// SPI cmd
//
#define RECIPROC_FREQ_MEAS_CMD_SPI_READ_STATUS 0x01
#define RECIPROC_FREQ_MEAS_CMD_SPI_CTRL 0x02
#define RECIPROC_FREQ_MEAS_CMD_SPI_SET_CFG_N 0x03
#define RECIPROC_FREQ_MEAS_CMD_SPI_SET_NCO_FREQ 0x04
#define RECIPROC_FREQ_MEAS_CMD_SPI_GET_DATA 0x10


//
// SPI sub cmd
//
#define RECIPROC_FREQ_MEAS_SUB_CMD_SIZE 3
#define RECIPROC_FREQ_MEAS_SUB_CMD_SPI_LED_ON 0x01
#define RECIPROC_FREQ_MEAS_SUB_CMD_SPI_LED_OFF 0x02
#define RECIPROC_FREQ_MEAS_SUB_CMD_SPI_LED_TOGGLE 0x03
#define RECIPROC_FREQ_MEAS_SUB_CMD_START_MEAS 0x04
#define RECIPROC_FREQ_MEAS_SUB_CMD_DUMMY 0x00
#define RECIPROC_FREQ_MEAS_SUB_CMD_CFG_N_100 0x00 //100
#define RECIPROC_FREQ_MEAS_SUB_CMD_CFG_N_1000 0x01 //1000
#define RECIPROC_FREQ_MEAS_SUB_CMD_CFG_N_10000 0x02 //10000
#define RECIPROC_FREQ_MEAS_SUB_CMD_CFG_N_100000 0x03 //100000
#define RECIPROC_FREQ_MEAS_SUB_CMD_CFG_N_1000000 0x04 //100000
#define RECIPROC_FREQ_MEAS_SUB_CMD_GET_MEAS_FPGA 0x00
#define RECIPROC_FREQ_MEAS_SUB_CMD_GET_MEAS_DELTA_TICK 0x01
#define RECIPROC_FREQ_MEAS_SUB_CMD_GET_MEAS_N_COUNTED  0x02



#define BYTE_SIZE_32b_word 4
#define RECIPROC_FREQ_MEAS_TX_BYTE_SIZE_32b_word  BYTE_SIZE_32b_word
#define RECIPROC_FREQ_MEAS_RX_BYTE_SIZE_32b_word  BYTE_SIZE_32b_word
#define RECIPROC_FREQ_MEAS_RX_BYTE_SIZE_4x32b_word  4*RECIPROC_FREQ_MEAS_RX_BYTE_SIZE_32b_word
#define RECIPROC_FREQ_MEAS_RX_BYTE_SIZE_8x32b_word  8*RECIPROC_FREQ_MEAS_RX_BYTE_SIZE_32b_word

#define  RECIPROC_FREQ_MEAS_BYTE_POS_MAJ_VERSION 0x00
#define  RECIPROC_FREQ_MEAS_BYTE_POS_MIN_VERSION 0x01
#define  RECIPROC_FREQ_MEAS_BYTE_POS_FIFO_COUNT 0x02 // not used for now, maybe later
#define  RECIPROC_FREQ_MEAS_BYTE_POS_STATUS 0x03
#define  RECIPROC_FREQ_MEAS_BIT_POS_STATUS_LED_OFF    BIT_2
#define  RECIPROC_FREQ_MEAS_BIT_POS_STATUS_LED_ON     BIT_3
#define  RECIPROC_FREQ_MEAS_BIT_POS_STATUS_NCO_FREQ_VALID	BIT_4
#define  RECIPROC_FREQ_MEAS_BIT_POS_STATUS_ERROR_FLAG	BIT_5
#define  RECIPROC_FREQ_MEAS_BIT_POS_STATUS_FIFO_FULL	BIT_6
#define  RECIPROC_FREQ_MEAS_BIT_POS_STATUS_START_MEAS	BIT_7

#define RECIPROC_FREQ_MEAS_FIFO_DEPTH 8
#define RECIPROC_FREQ_MEAS_FIFO_POS_START_TICK 8
#define RECIPROC_FREQ_MEAS_FIFO_POS_N_COUNTED 24

#define  RECIPROC_FREQ_MEAS_MAJ_VERSION 0x00
#define  RECIPROC_FREQ_MEAS_MIN_VERSION 0x03

typedef struct {
	uint32_t window_time_ms;
	uint32_t ref_fast_clk_MHz; 
	uint32_t square_out_freq;

} reciproc_freq_init_param;

struct reciproc_freq_platform_data {
	uint32_t window_time_ms;
	uint32_t ref_fast_clk_MHz; 
	uint32_t square_out_freq;
};

typedef struct {
    struct reciproc_freq_platform_data *pdata;
    pin_settings pins; // pin settings for SPI and GPIO
    bool led_state;
    bool dev_initialised;
} reciproc_freq_dev;


typedef struct reciproc_freq_cfg_s
{
    pin_settings pins; // pin settings for SPI and GPIO
    bool _spi_initialised; // SPI initalisation flag
    bool _ce_initialised; // CE initalisation flag
    bool _por_initialised;
    bool _enabled; // Device enabled flag
    reciproc_freq_dev *reciproc_freq_device;
} reciproc_freq_cfg_t;


/*!
    Enable reciproc_freq by pulling CE pin high
    @param pcfg pointer to reciproc_freq configuration struct
    @return None
*/
void reciproc_freq_enable(reciproc_freq_cfg_t *pcfg);

/*!
    Disable reciproc_freq output by pulling CE pin low
    The chip is still powered up, so all settings are
    maintained in this mode.
    @param pcfg pointer to reciproc_freq configuration struct
    @return None
*/
void reciproc_freq_disable(reciproc_freq_cfg_t *pcfg);

esp_err_t reciproc_freq_toggle_led(reciproc_freq_cfg_t *pcfg);
int32_t reciproc_freq_TEST_SET_FREQ(reciproc_freq_cfg_t *pcfg);
int32_t reciproc_freq_set_freq(reciproc_freq_cfg_t *pcfg, uint32_t nco_freq);
int32_t reciproc_freq_set_cfg_N(reciproc_freq_cfg_t *pcfg, uint32_t cfg_N);
esp_err_t reciproc_freq_start_meas(reciproc_freq_cfg_t *pcfg);
esp_err_t reciproc_freq_read_status(reciproc_freq_cfg_t *pcfg,uint8_t *rx_status);
esp_err_t reciproc_freq_read_delta_tick(reciproc_freq_cfg_t *pcfg,uint8_t *delta_tick, uint8_t nb_of_meas );
esp_err_t reciproc_freq_read_N_counted(reciproc_freq_cfg_t *pcfg,uint8_t *N_counted, uint8_t nb_of_meas );

int32_t reciproc_freq_setup(reciproc_freq_dev **device,reciproc_freq_init_param init_param);

/*!
    Initialise the reciproc_freq according to the configuration struct
    @param pcfg Pointer to the reciproc_freq configuration struct
    @return None
*/
void reciproc_freq_initialise(reciproc_freq_cfg_t *pcfg);
void stop_reciproc_freq_meas(reciproc_freq_cfg_t *pcfg);
