//
// SPI cmd
//
#define RECIPROC_FREQ_MEAS_CMD_SPI_READ_STATUS 0x01
#define RECIPROC_FREQ_MEAS_CMD_SPI_SET_LED_STATUS 0x02
#define RECIPROC_FREQ_MEAS_CMD_SPI_SET_NCO_FREQ 0x04
#define RECIPROC_FREQ_MEAS_CMD_SPI_GET_DATA 0x10



//
// SPI sub cmd
//
#define RECIPROC_FREQ_MEAS_SUB_CMD_SIZE 3
#define RECIPROC_FREQ_MEAS_SUB_CMD_SPI_LED_ON 0x01
#define RECIPROC_FREQ_MEAS_SUB_CMD_SPI_LED_OFF 0x02
#define RECIPROC_FREQ_MEAS_SUB_CMD_SPI_LED_TOGGLE 0x03
#define RECIPROC_FREQ_MEAS_SUB_CMD_DUMMY 0x00

#define BYTE_SIZE_32b_word 4
#define RECIPROC_FREQ_MEAS_TX_BYTE_SIZE_32b_word  BYTE_SIZE_32b_word
#define RECIPROC_FREQ_MEAS_RX_BYTE_SIZE_32b_word  BYTE_SIZE_32b_word
#define RECIPROC_FREQ_MEAS_RX_BYTE_SIZE_4x32b_word  4*RECIPROC_FREQ_MEAS_RX_BYTE_SIZE_32b_word
#define RECIPROC_FREQ_MEAS_RX_BYTE_SIZE_8x32b_word  8*RECIPROC_FREQ_MEAS_RX_BYTE_SIZE_32b_word


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

int32_t reciproc_freq_TEST_TOGGLE_LED(reciproc_freq_cfg_t *pcfg);
int32_t reciproc_freq_TEST_SET_FREQ(reciproc_freq_cfg_t *pcfg);
int32_t reciproc_freq_read_status(reciproc_freq_cfg_t *pcfg,uint8_t *rx_status);

int32_t reciproc_freq_setup(reciproc_freq_dev **device,reciproc_freq_init_param init_param);

/*!
    Initialise the reciproc_freq according to the configuration struct
    @param pcfg Pointer to the reciproc_freq configuration struct
    @return None
*/
void reciproc_freq_initialise(reciproc_freq_cfg_t *pcfg);
void stop_reciproc_freq_meas(reciproc_freq_cfg_t *pcfg);
