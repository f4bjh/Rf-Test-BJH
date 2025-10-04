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
int32_t reciproc_freq_TEST_WRITE_SPI_FPGA(reciproc_freq_cfg_t *pcfg);

int32_t reciproc_freq_setup(reciproc_freq_dev **device,reciproc_freq_init_param init_param);

/*!
    Initialise the reciproc_freq according to the configuration struct
    @param pcfg Pointer to the reciproc_freq configuration struct
    @return None
*/
void reciproc_freq_initialise(reciproc_freq_cfg_t *pcfg);
void stop_reciproc_freq_meas(reciproc_freq_cfg_t *pcfg);
