/*!
   @file adf4351.c
   @mainpage ADF4351 ESP32 library driver for Wideband Frequency Synthesizer
   @section intro_sec Introduction
   This is a C port of the original ADF4351 library made by David Fannin, KK6DF, et al.
   The driver has been tested on a ESP32-devkitc-v4 module connected to a Analog Devices ADF4351 eval board.
   License goes to the authors of the original library.

   The ADF4351 chip is a wideband freqency synthesizer integrated circuit that can generate frequencies
   from 35 MHz to 4.4 GHz. It incorporates a PLL (Fraction-N and Integer-N modes) and VCO, along with
   prescalers, dividers and multipiers. The users add a PLL loop filter and reference frequency to
   create a frequency generator with a very wide range, that is tuneable in settable frequency steps.
   The ADF4351 chip provides an I2C interface for setting the device registers that control the
   frequency and output levels, along with several IO pins for gathering chip status and
   enabling/disabling output and power modes.
   The ADF4351 library provides an Arduino API for accessing the features of the ADF chip.
   The basic PLL equations for the ADF4351 are:
   \f$ RF_{out} = f_{PFD} \times (INT +(\frac{FRAC}{MOD})) \f$
   where:
   \f$ f_{PFD} = REF_{IN} \times \left[ \frac{(1 + D)}{( R \times (1 + T))} \right]  \f$
   \f$ D = \textrm{RD2refdouble, ref doubler flag}\f$
   \f$ R = \textrm{RCounter, ref divider}\f$
   \f$ T = \textrm{RD1Rdiv2, ref divide by 2 flag}\f$
   @section author Author
   Yuting Xu
   @section license License
   MIT License
*/
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

static char TAG[] = "PLL";

uint32_t ADF4351_steps[] = {100, 500, 10000, 50000, 100000, 500000, 1000000};

spi_device_handle_t spi_handle;


#if 0
int ADF4351_set_freq(adf4351_cfg_t *pcfg, uint32_t freq)
{
    // input range checks
    // this will never trigger
    // if(freq > ADF_FREQ_MAX) 
    //     return -1;
    if(freq < ADF_FREQ_MIN) {
      ESP_LOGE(TAG,"output frequency too low");
      return -1;
    }

    // equation for RF_OUT
    // RF_OUT = [INT + (FRAC/MOD)] * (f_PFD/RF_DIVIDER)
    // RF_OUT is the RF frequency output
    // INT is the integer division facgtor
    // FRAC is the numerator of the fractional division (0 to MOD - 1)
    // MOD is the preset fractional modulus (2 to 4095)
    // RF_DIVIDER is the output dividers that divides down the VCO frequency
    // f_PFD = REF_IN * (1 + D)/(R * (1+T))

    int localosc_ratio = 2200000000 / freq;
    pcfg->_outdiv = 1;
    int RfDivSel = 0;

    // select the output divider
    while(pcfg->_outdiv <= localosc_ratio && pcfg->_outdiv <= 64)
    {
        pcfg->_outdiv *= 2;
        RfDivSel++;
    }

    if (freq > 3600000000/pcfg->_outdiv)
    {
        pcfg->Prescaler = 1;
    }
    else 
    {
        pcfg->Prescaler = 0;
    }

    double PFDFreq = pcfg->_reffreq * (1.0 + pcfg->RD2refdouble) / (pcfg->RCounter * (1.0 + pcfg->RD1Rdiv2)); // find the loop frequency

    double N = freq * pcfg->_outdiv / PFDFreq;
    pcfg->_N_Int = (uint16_t) N;
    double Mod = PFDFreq / pcfg->ChanStep;
    Mod = (double) ((uint32_t) Mod);
    double Frac = (N - pcfg->_N_Int) * Mod + 0.5;
    pcfg->_Frac = (uint32_t) Frac;
    pcfg->_Mod = (uint32_t) Mod;

    if(pcfg->_Frac != 0) 
    {
        uint32_t gcd = ADF4351_gcd_iter(pcfg->_Frac, pcfg->_Mod) ;

        if(gcd > 1) 
        {
            pcfg->_Frac /= gcd;
            Frac = (double) pcfg->_Frac;
            pcfg->_Mod /= gcd;
            Mod = (double) Mod;
        }
    }

    double cfreq;

    if(pcfg->_Frac == 0) 
    {
        cfreq = (PFDFreq * N) / pcfg->_outdiv;
    } 
    else 
    {
        cfreq = (PFDFreq * (N + (Frac/Mod))) / pcfg->_outdiv;
    }

    pcfg->_cfreq = cfreq;

    if(freq != (uint32_t) cfreq)
    {
        ESP_LOGI(TAG, "Calculated frequency (%f) different from input frequency ", cfreq);
    }
    
    // error checks
    if(pcfg->_Mod < 2 || pcfg->_Mod > 4095)
    {
        ESP_LOGE(TAG, "Mod out of range");
        return -1;
    }

    if(pcfg->_Frac > pcfg->_Mod - 1)
    {
        ESP_LOGE(TAG, "Frac out of range");
        return -1;
    }

    if((pcfg->Prescaler == 0) && (pcfg->_N_Int < 23))
    {
        ESP_LOGE(TAG, "N_int out of range");
        return -1;
    }

    else if((pcfg->Prescaler == 1) && (pcfg->_N_Int < 75))
    {
        ESP_LOGE(TAG, "N_int out of range");
        return -1;
    }

    // set the registers to 0 first
    memset(&pcfg->_registers, 0, sizeof(pcfg->_registers));

    // configure register 0
    // (0,3,0) control bits
    ADF4351_set_register_bf(&pcfg->_registers[0], 3, 12, pcfg->_Frac);
    ADF4351_set_register_bf(&pcfg->_registers[0], 15, 16, pcfg->_N_Int);
    ESP_LOGI(TAG, "N_Int calculated to be %d", pcfg->_N_Int);

    // configure register 1
    ADF4351_set_register_bf(&pcfg->_registers[1], 0, 3, 1);
    ADF4351_set_register_bf(&pcfg->_registers[1], 3, 12, pcfg->_Mod);
    ADF4351_set_register_bf(&pcfg->_registers[1], 15, 12, 1);
    ADF4351_set_register_bf(&pcfg->_registers[1], 27, 1, pcfg->Prescaler);
    // (28,1,0) phase adjust

    // configure register 2
    ADF4351_set_register_bf(&pcfg->_registers[2], 0, 3, 2);
    // (3,1,0) counter reset
    // (4,1,0) cp3 state
    // (5,1,0) power down
    ADF4351_set_register_bf(&pcfg->_registers[2], 6, 1, 1); // pd polarity
    if(pcfg->_Frac == 0)  
    {
        ADF4351_set_register_bf(&pcfg->_registers[2], 7, 1, 1); // LDP, int-n mode
        ADF4351_set_register_bf(&pcfg->_registers[2], 8, 1, 1); // ldf, int-n mode
    } 
    else 
    {
        ADF4351_set_register_bf(&pcfg->_registers[2], 7, 1, 0); // LDP, frac-n mode
        ADF4351_set_register_bf(&pcfg->_registers[2], 8, 1, 0); // ldf, frac-n mode
    }
    ADF4351_set_register_bf(&pcfg->_registers[2], 9, 4, 7); // charge pump
    // (13,1,0) dbl buf
    ADF4351_set_register_bf(&pcfg->_registers[2], 14, 10, pcfg->RCounter); // R counter
    ADF4351_set_register_bf(&pcfg->_registers[2], 24, 1, pcfg->RD1Rdiv2)  ; // RD1_RDiv2
    ADF4351_set_register_bf(&pcfg->_registers[2], 25, 1, pcfg->RD2refdouble)  ; // RD2refdouble
    // R[2].setbf(26,3,0) ; //  muxout, not used
    // (29,2,0) low noise and spurs mode

    // configure register 3
    ADF4351_set_register_bf(&pcfg->_registers[3], 0, 3, 3) ; // control bits
    ADF4351_set_register_bf(&pcfg->_registers[3], 3, 12, pcfg->ClkDiv) ; // clock divider
    // (15,2,0) clk div mode
    // (17,1,0) reserved
    // (18,1,0) CSR
    // (19,2,0) reserved
    if(pcfg->_Frac == 0 ) 
    {
        ADF4351_set_register_bf(&pcfg->_registers[3], 21, 1, 1); //  charge cancel, reduces pfd spurs
        ADF4351_set_register_bf(&pcfg->_registers[3], 22, 1, 1); //  ABP, int-n
    } 
    else  
    {
        ADF4351_set_register_bf(&pcfg->_registers[3], 21, 1, 0) ; //  charge cancel
        ADF4351_set_register_bf(&pcfg->_registers[3], 22, 1, 0); //  ABP, frac-n
    }
    ADF4351_set_register_bf(&pcfg->_registers[3], 23, 1, 1) ; // Band Select Clock Mode
    // (24,8,0) reserved

    // configure register 4
    ADF4351_set_register_bf(&pcfg->_registers[4], 0, 3, 4) ; // control bits
    ADF4351_set_register_bf(&pcfg->_registers[4], 3, 2, pcfg->pwrlevel) ; // output power 0-3 (-4dbM to 5dbM, 3db steps)
    ADF4351_set_register_bf(&pcfg->_registers[4], 5, 1, 1) ; // rf output enable
    // (6,2,0) aux output power
    // (8,1,0) aux output enable
    // (9,1,0) aux output select
    // (10,1,0) mtld
    // (11,1,0) vco power down
    ADF4351_set_register_bf(&pcfg->_registers[4], 12, 8, pcfg->BandSelClock) ; // band select clock divider
    ADF4351_set_register_bf(&pcfg->_registers[4], 20, 3, RfDivSel) ; // rf divider select
    ADF4351_set_register_bf(&pcfg->_registers[4], 23, 1, 1) ; // feedback select
    // (24,8,0) reserved

    // configure register 5
    ADF4351_set_register_bf(&pcfg->_registers[5], 0, 3, 5) ; // control bits
    // (3,16,0) reserved
    ADF4351_set_register_bf(&pcfg->_registers[5], 19, 2, 3) ; // Reserved field,set to 11
    // (21,1,0) reserved
    ADF4351_set_register_bf(&pcfg->_registers[5], 22, 2, 1) ; // LD Pin Mode
    // (24,8,0) reserved

    int i;
    for(i = 5; i > -1; i--)
    {
        ESP_LOGI(TAG, "Writing register with content 0x%" PRIx32 "", pcfg->_registers[i]);
        ADF4351_write_register(pcfg, pcfg->_registers[i]);
        ets_delay_us(1500); // why wait so long?
    }

    return 0;
}

int ADF4351_set_ref_freq(adf4351_cfg_t *pcfg, uint32_t ref_freq)
{
    // check if input is valid
    if(ref_freq > ADF_REFIN_MAX)
        return -1; // change return from 1 to -1 for clearer sign of error
    else if(ref_freq < 100000)
        return -1;

    float new_ref_freq = ref_freq * (1.0 + pcfg->RD2refdouble) / (pcfg->RCounter * (1.0 + pcfg->RD1Rdiv2));

    if(new_ref_freq > ADF_PFD_MAX)
        return -1;
    else if(ref_freq < ADF_PFD_MIN)
        return -1;

    pcfg->_reffreq = ref_freq;
    
    return 0;
}
#endif





void adf4351_write(adf4351_dev *dev, uint32_t reg)
{
    esp_err_t err;

    // ensure LE pin is low
    gpio_set_level(dev->pins.gpio_le, 0);
    ets_delay_us(5);

    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.tx_data[0] = (reg>>24)&0xff;
    t.tx_data[1] = (reg>>16)&0xff;
    t.tx_data[2] = (reg>> 8)&0xff;
    t.tx_data[3] = (reg    )&0xff;
    t.flags = SPI_TRANS_USE_TXDATA;
    t.length = 32;
    t.rx_buffer = NULL;

    err = spi_device_transmit(spi_handle, &t);
    if (err != ESP_OK)
      ESP_LOGE(TAG,"spi device transmit error %d", err);

    gpio_set_level(dev->pins.gpio_le, 1);    // pull LE pin high to latch incoming word into register
    ets_delay_us(5);
    gpio_set_level(dev->pins.gpio_le, 0);    // pull LE pin to its default low
}




/***************************************************************************//**
 * @brief Updates the registers values.
 *
 * @param dev - The device structure.
 *
 * @return Returns 0 in case of success or negative error code.
*******************************************************************************/
int32_t adf4351_sync_config(adf4351_dev *dev)
{
	int32_t ret, i, doublebuf = 0;

	for (i = ADF4351_REG5; i >= ADF4351_REG0; i--) {
		if ((dev->regs_hw[i] != dev->regs[i]) ||
		    ((i == ADF4351_REG0) && doublebuf)) {
			switch (i) {
			case ADF4351_REG1:
			case ADF4351_REG4:
				doublebuf = 1;
				break;
			}

			dev->val = (dev->regs[i] | i);
			adf4351_write(dev, dev->val);
			dev->regs_hw[i] = dev->regs[i];
		}
	}

	return 0;
}

/***************************************************************************//**
 * @brief Increases the R counter value until the ADF4351_MAX_FREQ_PFD is
 *        greater than PFD frequency.
 *
 * @param dev - The device structure.
 * @param r_cnt - Initial r_cnt value.
 *
 * @return Returns 0 in case of success or negative error code.
*******************************************************************************/
int32_t adf4351_tune_r_cnt(adf4351_dev *dev,
			   uint16_t r_cnt)
{
	do {
		r_cnt++;
		dev->fpfd = (dev->clkin * (dev->pdata->ref_doubler_en ? 2 : 1)) /
			    (r_cnt * (dev->pdata->ref_div2_en ? 2 : 1));
	} while (dev->fpfd > ADF4351_MAX_FREQ_PFD);

	return r_cnt;
}

/***************************************************************************//**
 * @brief Computes the greatest common divider of two numbers
 *
 * @return Returns the gcd.
*******************************************************************************/
uint32_t gcd(uint32_t x, uint32_t y)
{
	uint32_t tmp;

	while (y != 0) {
		tmp = x % y;
		x = y;
		y = tmp;
	}

	return x;
}

/***************************************************************************//**
 * @brief Sets the ADF4351 frequency.
 *
 * @param dev - The device structure.
 * @param freq - The desired frequency value.
 *
 * @return calculatedFrequency - The actual frequency value that was set.
*******************************************************************************/
int64_t adf4351_set_freq(adf4351_dev *dev,
			 uint64_t freq)
{
	uint64_t tmp;
	uint32_t div_gcd, prescaler, chspc;
	uint16_t mdiv, r_cnt = 0;
	uint8_t band_sel_div;
	int32_t ret;

	if ((freq > ADF4351_MAX_OUT_FREQ) || (freq < ADF4351_MIN_OUT_FREQ))
		return -1;

	if (freq > ADF4351_MAX_FREQ_45_PRESC) {
		prescaler = ADF4351_REG1_PRESCALER;
		mdiv = 75;
	} else {
		prescaler = 0;
		mdiv = 23;
	}

	dev->r4_rf_div_sel = 0;

	while (freq < ADF4351_MIN_VCO_FREQ) {
		freq <<= 1;
		dev->r4_rf_div_sel++;
	}

	/*
	 * Allow a predefined reference division factor
	 * if not set, compute our own
	 */
	if (dev->pdata->ref_div_factor)
		r_cnt = dev->pdata->ref_div_factor - 1;

	chspc = dev->chspc;

	do {
		do {
			do {
				r_cnt = adf4351_tune_r_cnt(dev, r_cnt);
				dev->r1_mod = dev->fpfd / chspc;
				if (r_cnt > ADF4351_MAX_R_CNT) {
					/* try higher spacing values */
					chspc++;
					r_cnt = 0;
				}
			} while ((dev->r1_mod > ADF4351_MAX_MODULUS) && r_cnt);
		} while (r_cnt == 0);


		tmp = freq * (uint64_t)dev->r1_mod + (dev->fpfd > 1);

		tmp = (tmp / dev->fpfd);	/* Div round closest (n + d/2)/d */

		dev->r0_fract = tmp % dev->r1_mod;
		tmp = tmp / dev->r1_mod;

		dev->r0_int = tmp;
	} while (mdiv > dev->r0_int);

	band_sel_div = dev->fpfd % ADF4351_MAX_BANDSEL_CLK >
		       ADF4351_MAX_BANDSEL_CLK / 2 ?
		       dev->fpfd / ADF4351_MAX_BANDSEL_CLK + 1 :
		       dev->fpfd / ADF4351_MAX_BANDSEL_CLK;

	if (dev->r0_fract && dev->r1_mod) {
		div_gcd = gcd(dev->r1_mod, dev->r0_fract);
		dev->r1_mod /= div_gcd;
		dev->r0_fract /= div_gcd;
	} else {
		dev->r0_fract = 0;
		dev->r1_mod = 1;
	}

	ESP_LOGI(TAG,"clkin=%lu chspc=%lu fpfd=%lu",dev->clkin,dev->chspc,dev->fpfd);
        ESP_LOGI(TAG,"rf_div=%lu ref_doubler=%d ref_div2=%d",dev->r4_rf_div_sel,(dev->pdata->ref_doubler_en ? 2 : 1), (dev->pdata->ref_div2_en ? 2 : 1));
	ESP_LOGI(TAG,"int=%lu frac=%lu mod=%lu",dev->r0_int,dev->r0_fract,dev->r1_mod);


	dev->regs[ADF4351_REG0] = ADF4351_REG0_INT(dev->r0_int) |
				  ADF4351_REG0_FRACT(dev->r0_fract);

	dev->regs[ADF4351_REG1] = ADF4351_REG1_PHASE(1) |
				  ADF4351_REG1_MOD(dev->r1_mod) |
				  prescaler;

	dev->regs[ADF4351_REG2] =
		ADF4351_REG2_10BIT_R_CNT(r_cnt) |
		ADF4351_REG2_DOUBLE_BUFF_EN |
		(dev->pdata->ref_doubler_en ? ADF4351_REG2_RMULT2_EN : 0) |
		(dev->pdata->ref_div2_en ? ADF4351_REG2_RDIV2_EN : 0) |
		(dev->pdata->r2_user_settings & (ADF4351_REG2_PD_POLARITY_POS |
				ADF4351_REG2_LDP_6ns | ADF4351_REG2_LDF_INT_N |
				ADF4351_REG2_CHARGE_PUMP_CURR_uA(5000) |
				ADF4351_REG2_MUXOUT(0x7) | ADF4351_REG2_NOISE_MODE(0x3)));

	dev->regs[ADF4351_REG3] = dev->pdata->r3_user_settings &
				  (ADF4351_REG3_12BIT_CLKDIV(0xFFF) |
				   ADF4351_REG3_12BIT_CLKDIV_MODE(0x3) |
				   ADF4351_REG3_12BIT_CSR_EN);

	dev->regs[ADF4351_REG4] =
		ADF4351_REG4_FEEDBACK_FUND |
		ADF4351_REG4_RF_DIV_SEL(dev->r4_rf_div_sel) |
		ADF4351_REG4_8BIT_BAND_SEL_CLKDIV(band_sel_div) |
		ADF4351_REG4_RF_OUT_EN |
		(dev->pdata->r4_user_settings &
		 (ADF4351_REG4_OUTPUT_PWR(0x3) |
		  ADF4351_REG4_AUX_OUTPUT_PWR(0x3) |
		  ADF4351_REG4_AUX_OUTPUT_EN |
		  ADF4351_REG4_AUX_OUTPUT_FUND |
		  ADF4351_REG4_MUTE_TILL_LOCK_EN));

	dev->regs[ADF4351_REG5] = ADF4351_REG5_LD_PIN_MODE_DIGITAL | 0x00180000;

	ret = adf4351_sync_config(dev);
	if (ret < 0) {
		return ret;
	}

	tmp = (uint64_t)((dev->r0_int * dev->r1_mod) +
			 dev->r0_fract) * (uint64_t)dev->fpfd;
	tmp = tmp / ((uint64_t)dev->r1_mod * ((uint64_t)1 << dev->r4_rf_div_sel));

	return tmp;
}

/***************************************************************************//**
 * @brief Initializes the ADF4351.
 *
 * @param device - The device structure.
 * @param init_param - The structure containing the device initial parameters.
 *
 * @return Returns 0 in case of success or negative error code.
*******************************************************************************/
int32_t adf4351_setup(adf4351_dev **device,
		      adf4351_init_param init_param)
{
	adf4351_dev *dev;
	int32_t ret=0;

	dev = (adf4351_dev *)malloc(sizeof(*dev));
	if (!dev) {
		return -1;
	}

	dev->pdata = (struct adf4351_platform_data *)malloc(sizeof(*dev->pdata));
	if (!dev->pdata)
		return -1;

	dev->pdata->clkin = init_param.clkin;

	dev->pdata->channel_spacing = init_param.channel_spacing;

	dev->pdata->power_up_frequency = init_param.power_up_frequency;
	dev->pdata->ref_div_factor = init_param.reference_div_factor;
	dev->pdata->ref_doubler_en = init_param.reference_doubler_enable;
	dev->pdata->ref_div2_en = init_param.reference_div2_enable;

	/* r2_user_settings */

	dev->pdata->r2_user_settings =
		init_param.phase_detector_polarity_positive_enable ?
		ADF4351_REG2_PD_POLARITY_POS : 0;
	dev->pdata->r2_user_settings |=
		init_param.lock_detect_precision_6ns_enable ?
		ADF4351_REG2_LDP_6ns : 0;
	dev->pdata->r2_user_settings |=
		init_param.lock_detect_function_integer_n_enable ?
		ADF4351_REG2_LDF_INT_N : 0;
	dev->pdata->r2_user_settings |=
		ADF4351_REG2_CHARGE_PUMP_CURR_uA(init_param.charge_pump_current);
	dev->pdata->r2_user_settings |=
		ADF4351_REG2_MUXOUT(init_param.muxout_select);
	dev->pdata->r2_user_settings |=
		init_param.low_spur_mode_enable ? ADF4351_REG2_NOISE_MODE(0x3) : 0;

	/* r3_user_settings */

	dev->pdata->r3_user_settings = init_param.cycle_slip_reduction_enable ?
				       ADF4351_REG3_12BIT_CSR_EN : 0;
	dev->pdata->r3_user_settings |= init_param.charge_cancellation_enable ?
					ADF4351_REG3_CHARGE_CANCELLATION_EN : 0;
	dev->pdata->r3_user_settings |= init_param.anti_backlash_3ns_enable ?
					ADF4351_REG3_ANTI_BACKLASH_3ns_EN : 0;
	dev->pdata->r3_user_settings |=
		init_param.band_select_clock_mode_high_enable ?
		ADF4351_REG3_BAND_SEL_CLOCK_MODE_HIGH : 0;
	dev->pdata->r3_user_settings |=
		ADF4351_REG3_12BIT_CLKDIV(init_param.clk_divider_12bit);
	dev->pdata->r3_user_settings |=
		ADF4351_REG3_12BIT_CLKDIV_MODE(init_param.clk_divider_mode);

	/* r4_user_settings */

	dev->pdata->r4_user_settings = init_param.aux_output_enable ?
				       ADF4351_REG4_AUX_OUTPUT_EN : 0;
	dev->pdata->r4_user_settings |= init_param.aux_output_fundamental_enable ?
					ADF4351_REG4_AUX_OUTPUT_FUND : 0;
	dev->pdata->r4_user_settings |= init_param.mute_till_lock_enable ?
					ADF4351_REG4_MUTE_TILL_LOCK_EN : 0;
	dev->pdata->r4_user_settings |=
		ADF4351_REG4_OUTPUT_PWR(init_param.output_power);
	dev->pdata->r4_user_settings |=
		ADF4351_REG4_AUX_OUTPUT_PWR(init_param.aux_output_power);

#if 0
	adf4351_out_altvoltage0_refin_frequency(dev, dev->pdata->clkin);
	adf4351_out_altvoltage0_frequency_resolution(dev,
			dev->pdata->channel_spacing);
	adf4351_out_altvoltage0_frequency(dev, dev->pdata->power_up_frequency);
#endif	

	*device = dev;

	ESP_LOGI(TAG,"ADF4351 successfully initialized.\n");

	return ret;
}

/***************************************************************************//**
 * @brief Stores PLL 0 frequency in Hz.
 *
 * @param dev - The device structure.
 * @param Hz - The selected frequency.
 *
 * @return Returns the selected frequency.
*******************************************************************************/
int64_t adf4351_out_altvoltage0_frequency(adf4351_dev *dev,
		int64_t Hz)
{
	return adf4351_set_freq(dev, Hz);
}

/***************************************************************************//**
 * @brief Stores PLL 0 frequency resolution/channel spacing in Hz.
 *
 * @param dev - The device structure.
 * @param Hz - The selected frequency.
 *
 * @return Returns the selected frequency.
*******************************************************************************/
int32_t adf4351_out_altvoltage0_frequency_resolution(adf4351_dev *dev,
		int32_t Hz)
{
	if (Hz != INT32_MAX) {
		dev->chspc = Hz;
	}

	return dev->chspc;
}

/***************************************************************************//**
 * @brief Sets PLL 0 REFin frequency in Hz.
 *
 * @param dev - The device structure.
 * @param Hz - The selected frequency.
 *
 * @return Returns the selected frequency.
*******************************************************************************/
int64_t adf4351_out_altvoltage0_refin_frequency(adf4351_dev *dev,
		int64_t Hz)
{
	if (Hz != INT32_MAX) {
		dev->clkin = Hz;
	}

	return dev->clkin;
}

/***************************************************************************//**
 * @brief Powers down the PLL.
 *
 * @param dev - The device structure.
 * @param pwd - Power option.
 *				Example: 0 - Power up the PLL.
 *						 1 - Power down the PLL.
 *
 * @return Returns the PLL's power status.
*******************************************************************************/
int32_t adf4351_out_altvoltage0_powerdown(adf4351_dev *dev,
		int32_t pwd)
{
	if (pwd == 1) {
		dev->regs[ADF4351_REG2] |= ADF4351_REG2_POWER_DOWN_EN;
		adf4351_sync_config(dev);
	}
	if (pwd == 0) {
		dev->regs[ADF4351_REG2] &= ~ADF4351_REG2_POWER_DOWN_EN;
		adf4351_sync_config(dev);
	}


	return (dev->regs[ADF4351_REG2] & ADF4351_REG2_POWER_DOWN_EN);
}



void adf4351_enable(adf4351_cfg_t *pcfg)
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

void adf4351_disable(adf4351_cfg_t *pcfg)
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

#if 0
void adf4351_set_register_bf(uint32_t *preg, uint8_t start, uint8_t len, uint32_t value)
{
    uint32_t bitmask = (1 << len) - 1;
    value &= bitmask;
    bitmask <<= start;
    *preg = (*preg & (~bitmask)) | (value << start);
}

uint32_t adf4351_get_register_bf(uint32_t *preg, uint8_t start, uint8_t len)
{
    uint32_t bitmask = ((1 << len) - 1) << start;
    uint32_t result = (*preg & bitmask) >> start;
    return result;
}
#endif


void adf4351_initialise(adf4351_cfg_t *pcfg)
{
    // initialise internal fields
    pcfg->_enabled = false;
    pcfg->_reffreq = REF_FREQ_DEFAULT;
    pcfg->_cfreq = 0.0;

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

    // GPIO settings for load enable pin (active high)
    gpio_config_t adf4351_le_cfg = 
    {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pull_down_en = 1,
        .pin_bit_mask = (1 << pcfg->pins.gpio_le)
    };

    // GPIO settings for chip enable pin (active high)
    gpio_config_t adf4351_ce_cfg = 
    {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pull_down_en = 1,
        .pin_bit_mask = (1 << pcfg->pins.gpio_ce)
    };

    // GPIO settings for lock detect pin (high - PLL lock, low - loss of PLL lock)
    gpio_config_t adf4351_ld_cfg = 
    {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = (1 << pcfg->pins.gpio_ld)
    };

	esp_err_t ret;

    ret = gpio_config(&adf4351_le_cfg);
    assert(ret == ESP_OK);
    pcfg->_le_initialised = true;

    ret = gpio_config(&adf4351_ce_cfg);
    assert(ret == ESP_OK);
    pcfg->_ce_initialised = true;

    ret = gpio_config(&adf4351_ld_cfg);
    assert(ret == ESP_OK);
    pcfg->_ld_initialised = true;

    // drive LE and CE low just in case
    gpio_set_level(pcfg->pins.gpio_le, 0);
    gpio_set_level(pcfg->pins.gpio_ce, 0);

    ESP_LOGI(TAG, "GPIO successfully initialised");

    ESP_LOGI(TAG, "SPI bus initialisation Mode: %d, Clock speed: %d, MOSI GPIO: %d", devcfg.mode, devcfg.clock_speed_hz, buscfg.mosi_io_num);
    ret = spi_bus_initialize(SENDER_HOST, &buscfg, SPI_DMA_DISABLED);
    assert(ret == ESP_OK);

    ESP_LOGI(TAG, "SPI bus successfully initialised");

    ret = spi_bus_add_device(SENDER_HOST, &devcfg, &spi_handle);
    assert(ret == ESP_OK);

    ESP_LOGI(TAG, "ADF4351 successfully attached");
    pcfg->_spi_initialised = true;
}


void adf4351_remove(adf4351_cfg_t *pcfg)
{
    esp_err_t ret;

    // Réinitialiser les GPIO
    if (pcfg->_le_initialised) {
        gpio_reset_pin(pcfg->pins.gpio_le);
        pcfg->_le_initialised = false;
    }

    if (pcfg->_ld_initialised) {
        gpio_reset_pin(pcfg->pins.gpio_ld);
        pcfg->_ld_initialised = false;
    }

    // Réinitialiser les autres champs
    pcfg->_reffreq = 0.0;
    pcfg->_cfreq = 0.0;
    pcfg->_enabled = false;

    // Supprimer le périphérique SPI
    if (pcfg->_spi_initialised) {
        spi_bus_remove_device(spi_handle);
        spi_bus_free(SENDER_HOST);
        pcfg->_spi_initialised = false;
    }



    ESP_LOGI(TAG, "ADF4351 successfully dettached");
}

