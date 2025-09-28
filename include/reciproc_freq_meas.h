#include "gpio.h"

#if 0
typedef struct {
	/* Device settings */
	uint32_t	clkin;
	uint32_t	channel_spacing;
	uint32_t	power_up_frequency;
	uint32_t	reference_div_factor;
	uint8_t		reference_doubler_enable;
	uint8_t		reference_div2_enable;

	/* r2_user_settings */
	uint8_t		phase_detector_polarity_positive_enable;
	uint8_t		lock_detect_precision_6ns_enable;
	uint8_t		lock_detect_function_integer_n_enable;
	uint32_t	charge_pump_current;
	uint32_t	muxout_select;
	uint8_t		low_spur_mode_enable;

	/* r3_user_settings */
	uint8_t		cycle_slip_reduction_enable;
	uint8_t		charge_cancellation_enable;
	uint8_t		anti_backlash_3ns_enable;
	uint8_t		band_select_clock_mode_high_enable;
	uint32_t	clk_divider_12bit;
	uint32_t	clk_divider_mode;

	/* r4_user_settings */
	uint8_t		aux_output_enable;
	uint8_t		aux_output_fundamental_enable;
	uint8_t		mute_till_lock_enable;
	uint32_t	output_power;
	uint32_t	aux_output_power;
} reciproc_freq_init_param;

typedef struct {
	struct reciproc_freq_platform_data *pdata;
        pin_settings pins; // pin settings for SPI and GPIO
	uint32_t	clkin;
	uint32_t	chspc;	/* Channel Spacing */
	uint32_t	fpfd;	/* Phase Frequency Detector */
	uint32_t	min_out_freq;
	uint32_t	r0_fract;
	uint32_t	r0_int;
	uint32_t	r1_mod;
	uint32_t	r4_rf_div_sel;
	uint32_t	regs[6];
	uint32_t	regs_hw[6];
	uint32_t 	val;
} reciproc_freq_dev;
#endif
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
    bool _enabled; // Device enabled flag
    reciproc_freq_dev *reciproc_freq_device;
} reciproc_freq_cfg_t;

#if 0
struct reciproc_freq_platform_data {
	uint32_t	clkin;
	uint32_t	channel_spacing;
	uint64_t	power_up_frequency;

	uint16_t	ref_div_factor; /* 10-bit R counter */
	uint8_t	    ref_doubler_en;
	uint8_t	    ref_div2_en;

	uint32_t    r2_user_settings;
	uint32_t    r3_user_settings;
	uint32_t    r4_user_settings;
	int32_t	    gpio_lock_detect;
};
#endif

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
