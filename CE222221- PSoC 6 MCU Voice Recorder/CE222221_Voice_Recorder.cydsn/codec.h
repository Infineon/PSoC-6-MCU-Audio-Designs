/*****************************************************************************
* File Name: codec.h
*
* Version: 1.0
*
* Description: This file contains the function prototypes and constants used in
*  codec.c. This driver is intended for AK4954A.
*
******************************************************************************
* Copyright (2018), Cypress Semiconductor Corporation.
******************************************************************************
* This software is owned by Cypress Semiconductor Corporation (Cypress) and is
* protected by and subject to worldwide patent protection (United States and
* foreign), United States copyright laws and international treaty provisions.
* Cypress hereby grants to licensee a personal, non-exclusive, non-transferable
* license to copy, use, modify, create derivative works of, and compile the
* Cypress Source Code and derivative works for the sole purpose of creating
* custom software in support of licensee product to be used only in conjunction
* with a Cypress integrated circuit as specified in the applicable agreement.
* Any reproduction, modification, translation, compilation, or representation of
* this software except as specified above is prohibited without the express
* written permission of Cypress.
*
* Disclaimer: CYPRESS MAKES NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, WITH
* REGARD TO THIS MATERIAL, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
* Cypress reserves the right to make changes without further notice to the
* materials described herein. Cypress does not assume any liability arising out
* of the application or use of any product or circuit described herein. Cypress
* does not authorize its products for use as critical components in life-support
* systems where a malfunction or failure may reasonably be expected to result in
* significant injury to the user. The inclusion of Cypress' product in a life-
* support systems application implies that the manufacturer assumes all risk of
* such use and in doing so indemnifies Cypress against all charges. Use may be
* limited by and subject to the applicable Cypress software license agreement.
*****************************************************************************/

#ifndef CODEC_H
	#define CODEC_H	
	
    #include <stdint.h>    
    
    /* I2C Address of the Codex */
	#define CODEC_I2C_ADDR			(0x12u)

    #define CODEC_PACKET_SIZE       (0x02u)

    /* Timeout in Milliseconds for I2C commands */
    #define CODEC_I2C_TIMEOUT       (50u)
    
	/**************************************************************************************************
	* Register Addresses for Codec I2C Interface
	**************************************************************************************************/

	#define CODEC_REG_PWR_MGMT1    	0x00    /* Power Management 1 */
	#define CODEC_REG_PWR_MGMT2    	0x01    /* Power Management 2 */
	#define CODEC_REG_SIG_SEL1    	0x02    /* Signal Select 1 */
	#define CODEC_REG_SIG_SEL2 	   	0x03    /* Signal Select 2 */
	#define CODEC_REG_SIG_SEL3     	0x04    /* Signal Select 3 */
	#define CODEC_REG_MODE_CTRL1 	0x05    /* Mode Control 1 */
	#define CODEC_REG_MODE_CTRL2   	0x06    /* Mode Control 2 */
	#define CODEC_REG_MODE_CTRL3   	0x07    /* Mode Control 3 */
	#define CODEC_REG_DIG_MIC   	0x08    /* Digital Microphone */
	#define CODEC_REG_TMR_SEL     	0x09    /* Timer Select */
	#define CODEC_REG_LCH_IN_VOL    0x0D    /* Left Channel Input Volume Control */
    #define CODEC_REG_RCH_IN_VOL    0x0E    /* Right Channel Input Volume Control */
    #define CODEC_REG_HI_OUT_CTRL   0x12    /* High Pass Filter Output Control */
    #define CODEC_REG_LCH_DIG_VOL   0x13    /* Left Channel Digital Volume Control */
    #define CODEC_REG_RCH_DIG_VOL   0x14    /* Left Channel Digital Volume Control */
    #define CODEC_REG_BEEP_FREQ     0x15    /* BEEP Frequency */
    #define CODEC_REG_BEEP_ON_TIME  x016    /* BEEP ON Time */
    #define CODEC_REG_BEEP_OFF_TIME 0x17    /* BEEP OFF Time */
    #define CODEC_REG_BEEP_RPT_CNT  0x18    /* BEEP Repeat Count */
    #define CODEC_REG_VOL_CTRL      0x19    /* BEEP Volume Control */
    #define CODEC_REG_DIG_FILT_MODE 0x1D    /* Digital Filter Mode */
   
	/* Register bit settings for CODEC_REG_PWR_MGMT1 register */	
	#define CODEC_PWR_MGMT1_PMADL	0x01    /* Microphone Amplifier Lch and ADC Lch Power Management */
	#define CODEC_PWR_MGMT1_PMADR   0x02    /* Microphone Amplifier Rch and ADC Rch Power Management */
	#define CODEC_PWR_MGMT1_PMDAC   0x04    /* DAC Power Management */
    #define CODEC_PWR_MGMT1_LSV     0x08    /* Low Voltage Operation Mode of the Speaker Amplifier */
    #define CODEC_PWR_MGMT1_PMBP    0x20    /* BEEP Generating Circuit Power Management */
    #define CODEC_PWR_MGMT1_PMVCM   0x40    /* VCOM Power Management */
    #define CODEC_PWR_MGMT1_PMPFIL  0x80    /* Programmable Filter Block Power Management */
    
	/* Register bit settings for CODEC_REG_PWR_MGMT2 register */	
	#define CODEC_PWR_MGMT2_LOSEL	0x01    /* Stereo Line Output Select */
    #define CODEC_PWR_MGMT2_PMSL	0x02    /* Speaker Amplifier or Stereo Line Output Power Management */
    #define CODEC_PWR_MGMT2_PMPLL	0x04    /* PLL Power Management */
    #define CODEC_PWR_MGMT2_MS 	    0x08    /* Master/Slave Mode Select */
    #define CODEC_PWR_MGMT2_PMHPL	0x10    /* Lch Headphone Amplifier and Charge Pump Power Management */
    #define CODEC_PWR_MGMT2_PMHPR	0x20    /* Rch Headphone Amplifier and Charge Pump Power Management */
	
	/* Register bit settings for CODEC_REG_SIG_SEL1 register */	
	#define CODEC_SIG_SEL1_MGAIN_0dB  	0x04    /* Microphone Amplifier Gain Control equal 0dB */
    #define CODEC_SIG_SEL1_MGAIN_6dB  	0x00    /* Microphone Amplifier Gain Control equal +6dB */
    #define CODEC_SIG_SEL1_MGAIN_13dB  	0x01    /* Microphone Amplifier Gain Control equal +13dB */
    #define CODEC_SIG_SEL1_MGAIN_20dB  	0x02    /* Microphone Amplifier Gain Control equal +20dB */
    #define CODEC_SIG_SEL1_MGAIN_26dB  	0x03    /* Microphone Amplifier Gain Control equal +26dB */
    #define CODEC_SIG_SEL1_PMMP  	    0x08    /* MPWR pin Power Management */
    #define CODEC_SIG_SEL1_MPSEL  	    0x10    /* MPWR Output Select */
    #define CODEC_SIG_SEL1_DACSL  	    0x20    /* Signal Switch Control from DAC to Speaker or Stereo Line Amplifier */
    #define CODEC_SIG_SEL1_SLPSN  	    0x80    /* Speaker or Stereo Line Amplifier Power-Save Mode */
	
	/* Register bit settings for CODEC_REG_SIG_SEL2 register */	
	#define CODEC_SIG_SEL2_INR1  	0x00    /* ADC Rch Input Source Select (1) */
    #define CODEC_SIG_SEL2_INR2  	0x01    /* ADC Rch Input Source Select (2) */
    #define CODEC_SIG_SEL2_INR3  	0x02    /* ADC Rch Input Source Select (3) */
    #define CODEC_SIG_SEL2_INL1  	0x00    /* ADC Lch Input Source Select (1) */
    #define CODEC_SIG_SEL2_INL2  	0x04    /* ADC Lch Input Source Select (2) */
    #define CODEC_SIG_SEL2_INL3  	0x08    /* ADC Lch Input Source Select (3) */
    #define CODEC_SIG_SEL2_SLG_0db  0x00    /* Stereo Line Amplifier Output Gain 0dB */
    #define CODEC_SIG_SEL2_SLG_2db  0x40    /* Stereo Line Amplifier Output Gain +2dB */
    #define CODEC_SIG_SEL2_SLG_4db  0x80    /* Stereo Line Amplifier Output Gain +4dB */
    #define CODEC_SIG_SEL2_SLG_6db  0xC0    /* Stereo Line Amplifier Output Gain +6dB */
	
    /* Register bit settings for CODEC_REG_SIG_SEL3 register */
    #define CODEC_SIG_SEL3_MONO     0x01    /* Monaural mixing setting of the DAC output */
    #define CODEC_SIG_SEL3_MOFF     0x02    /* Soft Transition Control of "BEEP->Headphone" Connection ON/OFF */
    #define CODEC_SIG_SEL3_PTS_1X   0x00    /* Soft Transition Time of "BEEP->Headphone" Connection ON/OFF (1) */
    #define CODEC_SIG_SEL3_PTS_2X   0x04    /* Soft Transition Time of "BEEP->Headphone" Connection ON/OFF (2) */
    #define CODEC_SIG_SEL3_PTS_4X   0x08    /* Soft Transition Time of "BEEP->Headphone" Connection ON/OFF (3) */
    #define CODEC_SIG_SEL3_PTS_8X   0x0C    /* Soft Transition Time of "BEEP->Headphone" Connection ON/OFF (4) */
    
	/* Register bit settings for CODEC_REG_MODE_CTRL1 register */	
                                                    /* Audio Interface Format */
	#define CODEC_MODE_CTRL1_DIF_24M_24L    0x00    /* 24-bit MSB / 24-bit LSB */
    #define CODEC_MODE_CTRL1_DIF_24M_16L    0x01    /* 24-bit MSB / 16-bit LSB */
    #define CODEC_MODE_CTRL1_DIF_24M_24M    0x02    /* 24-bit MSB / 24-bit MSB */
    #define CODEC_MODE_CTRL1_DIF_24_16_I2S  0x03    /* 24/16-bit I2S Compatible */
    #define CODEC_MODE_CTRL1_DIF_32M_32M    0x06    /* 32-bit MSB / 32-bit MSB */
    #define CODEC_MODE_CTRL1_DIF_32_I2S     0x07    /* 32-bit I2S Compatible */
	#define CODEC_MODE_CTRL1_BCK0_32fs      0x00    /* BICK Output Frequency of 32fs */
    #define CODEC_MODE_CTRL1_BCK0_64fs      0x08    /* BICK Output Frequency of 64fs */
    #define CODEC_MODE_CTRL1_PLL_32fs       0x00    /* PLL Clock: BICK at 32fs */
    #define CODEC_MODE_CTRL1_PLL_64fs       0x00    /* PLL Clock: BICK at 64fs */
    #define CODEC_MODE_CTRL1_PLL_11M2986Hz  0x00    /* PLL Clock: MCKI at 11.2896MHz */
    #define CODEC_MODE_CTRL1_PLL_12M288Hz   0x00    /* PLL Clock: MCKI at 12.288MHz */
    #define CODEC_MODE_CTRL1_PLL_12MHz      0x00    /* PLL Clock: MCKI at 12MHz */
    #define CODEC_MODE_CTRL1_PLL_24MHz      0x00    /* PLL Clock: MCKI at 24MHz */
    #define CODEC_MODE_CTRL1_PLL_13M5Hz     0x00    /* PLL Clock: MCKI at 13.5MHz */
    #define CODEC_MODE_CTRL1_PLL_27MHz      0x00    /* PLL Clock: MCKI at 27MHz */
	
	/* Register bit settings for CODEC_REG_MODE_CTRL2 register */	
	#define CODEC_MODE_CTRL2_FS_8kHz        0x00    /* Sampling Rate at 8kHz */
    #define CODEC_MODE_CTRL2_FS_11k025Hz    0x01    /* Sampling Rate at 11.025kHz */
    #define CODEC_MODE_CTRL2_FS_12kHz       0x02    /* Sampling Rate at 12kHz */
    #define CODEC_MODE_CTRL2_FS_16kHz       0x04    /* Sampling Rate at 16kHz */
    #define CODEC_MODE_CTRL2_FS_22k05Hz     0x05    /* Sampling Rate at 22.05kHz */
    #define CODEC_MODE_CTRL2_FS_24kHz       0x06    /* Sampling Rate at 24kHz */
    #define CODEC_MODE_CTRL2_FS_32kHz       0x08    /* Sampling Rate at 32kHz */
    #define CODEC_MODE_CTRL2_FS_44k1Hz      0x09    /* Sampling Rate at 44.1kHz */
    #define CODEC_MODE_CTRL2_FS_48kHz       0x0A    /* Sampling Rate at 48kHz */
    #define CODEC_MODE_CTRL2_FS_64kHz       0x0C    /* Sampling Rate at 64kHz */
    #define CODEC_MODE_CTRL2_FS_88k2Hz      0x0D    /* Sampling Rate at 88.2kHz */
    #define CODEC_MODE_CTRL2_FS_96kHz       0x0E    /* Sampling Rate at 96kHz */
	#define CODEC_MODE_CTRL2_CM_256fs       0x00    /* MCKI Input Frequency at 256fs */
    #define CODEC_MODE_CTRL2_CM_384fs       0x40    /* MCKI Input Frequency at 384fs */
    #define CODEC_MODE_CTRL2_CM_512fs       0x80    /* MCKI Input Frequency at 512fs */
    #define CODEC_MODE_CTRL2_CM_1024fs      0xC0    /* MCKI Input Frequency at 1024fs */
	
	/* Register bit settings for CODEC_REG_MODE_CTRL3 register */	
	#define CODEC_MODE_CTRL3_LPDA   0x01    /* Low-Power Consumption Mode of DAC + HP */
    #define CODEC_MODE_CTRL3_LPMIC  0x02    /* Low-Power Consumption Mode of Microphone Amplifier */
    #define CODEC_MODE_CTRL3_IVOLC  0x04    /* Input Digital Volume Control Mode Select */
    #define CODEC_MODE_CTRL3_DVOLC  0x10    /* Output Digital Volume 2 Control Mode Select */
    #define CODEC_MODE_CTRL3_SMUTE  0x20    /* Soft Mute Control */
    #define CODEC_MODE_CTRL3_THDET  0x40    /* Thermal Shutdown Detection Result */
    #define CODEC_MODE_CTRL3_OVFL   0x80    /* ADC Overflow Output Enable (OVF Pin) */
	
	/* Register bit settings for CODEC_REG_DIG_FILT_MODE register */	
	#define CODEC_DIG_FILT_MODE_PFSDO   0x01    /* SDTO Output Signal Select */
    #define CODEC_DIG_FILT_MODE_ADCPF   0x02    /* Programmable Filter / ALC Input Signal Select */
    #define CODEC_DIG_FILT_MODE_PFDAC   0x04    /* DAC Input Signal Select */
    #define CODEC_DIG_FILT_MODE_PMDRC   0x80    /* Dynamic Range Control Circuit Power Management */
    
	/* Register bit settings for CODEC_REG_HI_OUT_CTRL register */	
	#define CODEC_HI_OUT_CTRL_HPZ   0x08    /* Pull-down Setting of HP Amplifier */
			
	/* Register settings for the Codec chip */
	#define CODEC_RESET_WAIT_DELAY				(10)	 /* in milli seconds */

    /* Volume Control Constants */
	#define CODEC_HP_DEFAULT_VOLUME				(0x0C)  /* Default Value (0.0dB) */
	#define CODEC_HP_VOLUME_MAX					(0x00) 	/* Maximum Value (+6.0dB) */	
    #define CODEC_HP_VOLUME_MIN					(0x8F) 	/* Minimum Value (-65.5dB) */	
	#define CODEC_HP_MUTE_VALUE					(0x90) 	/* Writing <= 0x00 mutes the headphone output */
		
    /* Default Configuration Values */
	#define CODEC_DEF_SAMPLING_RATE				CODEC_MODE_CTRL2_CM_256fs
		
    /* Delay function */
    #define CODEC_DELAY_FUNC                    vTaskDelay(1)
    
	uint32_t Codec_Init(void);
	uint32_t Codec_AdjustBothHeadphoneVolume(uint8_t volume);
	uint32_t Codec_Activate(void);
	uint32_t Codec_Deactivate(void);
	uint32_t Codec_SendData(uint8_t regAddr, uint8_t data);
	
#endif /* #ifndef CODEC_H */

/* [] END OF FILE */
