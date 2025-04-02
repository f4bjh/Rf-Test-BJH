#ifndef adf4351_H
#define adf4351_H  

#define ADF_FREQ_MAX 4294967295         ///< Maximum Generated Frequency
#define ADF_FREQ_MIN  34385000          ///< Minimum Generated Frequency
#define ADF_PFD_MAX   32000000.0        ///< Maximum Frequency for Phase Detector
#define ADF_PFD_MIN   125000.0          ///< Minimum Frequency for Phase Detector
#define ADF_REFIN_MAX   250000000       ///< Maximum Reference Frequency
#define REF_FREQ_DEFAULT 100000000      ///< Default Reference Frequency

#define SENDER_HOST SPI3_HOST

/* Channels */
#define ADF4351_RX_CHANNEL	0
#define ADF4351_TX_CHANNEL	1

/* Registers */
#define ADF4351_REG0	0
#define ADF4351_REG1	1
#define ADF4351_REG2	2
#define ADF4351_REG3	3
#define ADF4351_REG4	4
#define ADF4351_REG5	5

/* REG0 Bit Definitions */
#define ADF4351_REG0_FRACT(x)					(((x) & 0xFFF) << 3)
#define ADF4351_REG0_INT(x)						(((x) & 0xFFFF) << 15)

/* REG1 Bit Definitions */
#define ADF4351_REG1_MOD(x)						(((x) & 0xFFF) << 3)
#define ADF4351_REG1_PHASE(x)					(((x) & 0xFFF) << 15)
#define ADF4351_REG1_PRESCALER					(1 << 27)

/* REG2 Bit Definitions */
#define ADF4351_REG2_COUNTER_RESET_EN			(1 << 3)
#define ADF4351_REG2_CP_THREESTATE_EN			(1 << 4)
#define ADF4351_REG2_POWER_DOWN_EN				(1 << 5)
#define ADF4351_REG2_PD_POLARITY_POS			(1 << 6)
#define ADF4351_REG2_LDP_6ns					(1 << 7)
#define ADF4351_REG2_LDP_10ns					(0 << 7)
#define ADF4351_REG2_LDF_FRACT_N				(0 << 8)
#define ADF4351_REG2_LDF_INT_N					(1 << 8)
#define ADF4351_REG2_CHARGE_PUMP_CURR_uA(x)		(((((x)-312) / 312) & 0xF) << 9)
#define ADF4351_REG2_DOUBLE_BUFF_EN				(1 << 13)
#define ADF4351_REG2_10BIT_R_CNT(x)				((x) << 14)
#define ADF4351_REG2_RDIV2_EN					(1 << 24)
#define ADF4351_REG2_RMULT2_EN					(1 << 25)
#define ADF4351_REG2_MUXOUT(x)					((x) << 26)
#define ADF4351_REG2_NOISE_MODE(x)				(((x) & 0x3) << 29)

/* REG3 Bit Definitions */
#define ADF4351_REG3_12BIT_CLKDIV(x)			((x) << 3)
#define ADF4351_REG3_12BIT_CLKDIV_MODE(x)		((x) << 16)
#define ADF4351_REG3_12BIT_CSR_EN				(1 << 18)
#define ADF4351_REG3_CHARGE_CANCELLATION_EN		(1 << 21)
#define ADF4351_REG3_ANTI_BACKLASH_3ns_EN		(1 << 22)
#define ADF4351_REG3_BAND_SEL_CLOCK_MODE_HIGH	(1 << 23)

/* REG4 Bit Definitions */
#define ADF4351_REG4_OUTPUT_PWR(x)				((x) << 3)
#define ADF4351_REG4_RF_OUT_EN					(1 << 5)
#define ADF4351_REG4_AUX_OUTPUT_PWR(x)			((x) << 6)
#define ADF4351_REG4_AUX_OUTPUT_EN				(1 << 8)
#define ADF4351_REG4_AUX_OUTPUT_FUND			(1 << 9)
#define ADF4351_REG4_AUX_OUTPUT_DIV				(0 << 9)
#define ADF4351_REG4_MUTE_TILL_LOCK_EN			(1 << 10)
#define ADF4351_REG4_VCO_PWRDOWN_EN				(1 << 11)
#define ADF4351_REG4_8BIT_BAND_SEL_CLKDIV(x)	((x) << 12)
#define ADF4351_REG4_RF_DIV_SEL(x)				((x) << 20)
#define ADF4351_REG4_FEEDBACK_DIVIDED			(0 << 23)
#define ADF4351_REG4_FEEDBACK_FUND				(1 << 23)

/* REG5 Bit Definitions */
#define ADF4351_REG5_LD_PIN_MODE_LOW			(0 << 22)
#define ADF4351_REG5_LD_PIN_MODE_DIGITAL		(1 << 22)
#define ADF4351_REG5_LD_PIN_MODE_HIGH			(3 << 22)

/* Specifications */
#define ADF4351_MAX_OUT_FREQ		4400000000ULL /* Hz */
#define ADF4351_MIN_OUT_FREQ		34375000 /* Hz */
#define ADF4351_MIN_VCO_FREQ		2200000000ULL /* Hz */
#define ADF4351_MAX_FREQ_45_PRESC	3000000000ULL /* Hz */
#define ADF4351_MAX_FREQ_PFD		32000000 /* Hz */
#define ADF4351_MAX_BANDSEL_CLK		125000 /* Hz */
#define ADF4351_MAX_FREQ_REFIN		250000000 /* Hz */
#define ADF4351_MAX_MODULUS			4095
#define ADF4351_MAX_R_CNT			1023

typedef struct 
{
    uint8_t gpio_sclk; // CLK pin
    uint8_t gpio_miso; // data pin
    uint8_t gpio_mosi; // dummy pin
    uint8_t gpio_cs; // dummy pin
    uint8_t gpio_ce; // chip enable
    uint8_t gpio_le; // load enable
    uint8_t gpio_ld; // lock detect
} pin_settings;

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
} adf4351_init_param;

typedef struct {
	struct adf4351_platform_data *pdata;
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
} adf4351_dev;

// you should not access variables that starts with an underscore, these are reserved for internal calculations
typedef struct adf4351_cfg_s
{
    uint32_t _reffreq; // always use adf4351_set_ref_freq to access this variable, only set this directly if you know that you are doing
    double _cfreq; // this is the calculated frequency (useful for debugging when output frequency is wrong). This value is overwritten each time AD5941_set_freq() is called.
    int _outdiv; // the PLL output divider value for the current frequency. This value is overwritten each time AD5941_set_freq() is called.
    uint8_t RD2refdouble; // the PLL reference frequencer doubler flag. It is used to double the input reference frequency. Set to 0 or 1.
    int RCounter; // the PLL R counter value for the current frequency 10bit counter used to divide the ref freq for the PFD
    uint8_t RD1Rdiv2; // the PLL reference freuqency divider by 2. Sets a divide-by-2 relationship between Rcounter and PFD.
    uint8_t BandSelClock; // the PLL Band Select Clock Value
    int ClkDiv; // The PLL ref freq prescaler (divider) flag. Set to 0 or 1
    uint8_t Prescaler; // The PLL ref freq prescaler (divider) flag. Set to 0 or 1
    double _PFDFreq; // The PLL Phase Detect Freq  value for the current frequency. Can be changed if a new reference frequency is used.
    uint32_t ChanStep; // The channel step value. Can be directly changed to set a new frequency step from the defined steps[] array. You should not set an arbitrary value.
    uint32_t _Mod; // The PLL Mod value for the current frequency. this value is overwritten each time AD5941_set_freq() is called.
    int _Frac; //  The PLL Frac value for the current frequency. This value is overwritten each time sef() is called.
    uint16_t _N_Int; //  The PLL INT value for the current frequency. This value is overwritten each time sef() is called.
    uint8_t pwrlevel; // The power output level settings. Allowed values 0-4
    pin_settings pins; // pin settings for SPI and GPIO
    bool _spi_initialised; // SPI initalisation flag
    bool _ce_initialised; // CE initalisation flag
    bool _le_initialised; // LE initalisation flag
    bool _ld_initialised; // LD initalisation flag
    bool _enabled; // Device enabled flag
    adf4351_dev *adf4351_device;
} adf4351_cfg_t;

struct adf4351_platform_data {
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



/*!
    gets the current register bitfield value, based on the start and length mask
    @param preg pointer to register
    @param start index of the bit of where to start
    @param len length number of bits to get
    @return bitfield value
*/
uint32_t adf4351_get_register_bf(uint32_t *preg, uint8_t start, uint8_t len);

/*!
    modifies the register value based on a value and bitfield (mask)
    @param preg pointer to register
    @param start index of the bit to start the modification
    @param len length number of bits to modify
    @param value value to modify (value is truncated if larger than len bits)
*/
void adf4351_set_register_bf(uint32_t *preg, uint8_t start, uint8_t len, uint32_t value);

/*!
    calculates the greatest common denominator for two values
    helper function to calculate PLL values
    @param u value 1
    @param v value 2
    @return the greatest common denominator
*/
uint32_t adf4351_gcd_iter(uint32_t u, uint32_t v);

/*!
    Enable adf4351 by pulling CE pin high
    @param pcfg pointer to adf4351 configuration struct
    @return None
*/
void adf4351_enable(adf4351_cfg_t *pcfg);

/*!
    Disable adf4351 output by pulling CE pin low
    The chip is still powered up, so all settings are
    maintained in this mode.
    @param pcfg pointer to adf4351 configuration struct
    @return None
*/
void adf4351_disable(adf4351_cfg_t *pcfg);

/*!
    Initialise the adf4351 according to the configuration struct
    @param pcfg Pointer to the adf4351 configuration struct
    @return None
*/
void adf4351_initialise(adf4351_cfg_t *pcfg);

/*!
    sets the incoming reference frequency to the adf4351 chip,
    based on your OXCO or other freqeuncy reference source value.
    Checks for min and max settings
    @param pcfg Pointer to the adf4351 configuration struct
    @param ref_freq new reference frequency
    @return success (0) or fail (-1)
*/
int adf4351_set_ref_freq(adf4351_cfg_t *pcfg, uint32_t ref_freq);

/*!
    sets the output frequency
    automatically calculates the PLL and other parameters
    returns false if the desired frequency does not match the
    calculated frequency or is out of range.
    @param pcfg pointer to adf4351 configuration struct
    @param freq target frequency
    @return success (0) or fail (-1)
*/
int64_t adf4351_set_freq(adf4351_dev *dev, uint64_t freq);

/*! Initializes the ADF4351. */
int32_t adf4351_setup(adf4351_dev **device,
		      adf4351_init_param init_param);
/*! Writes 4 bytes of data to ADF4351. */
void adf4351_write(adf4351_dev *dev, uint32_t reg);
/*! Stores PLL 0 frequency in Hz. */
int64_t adf4351_out_altvoltage0_frequency(adf4351_dev *dev,
		int64_t Hz);
/*! Stores PLL 0 frequency resolution/channel spacing in Hz. */
int32_t adf4351_out_altvoltage0_frequency_resolution(adf4351_dev *dev,
		int32_t Hz);
/*! Sets PLL 0 REFin frequency in Hz. */
int64_t adf4351_out_altvoltage0_refin_frequency(adf4351_dev *dev,
		int64_t Hz);
/*! Powers down the PLL.  */
int32_t adf4351_out_altvoltage0_powerdown(adf4351_dev *dev,
		int32_t pwd);

#endif
