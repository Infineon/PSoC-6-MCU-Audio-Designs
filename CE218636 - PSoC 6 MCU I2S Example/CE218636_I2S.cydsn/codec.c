/*******************************************************************************
* File Name: codec.c
*
* Description: This file contains the codec control APIs
*
******************************************************************************
* Copyright (2017), Cypress Semiconductor Corporation.
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

#include "codec.h"
#include "CodecI2CM.h"
#include "stdbool.h"
#include "gpio/cy_gpio.h"

#define I2C_WRITE_OPERATION		(0x00)

/*******************************************************************************
* Global variables
*******************************************************************************/
/* Structure for master transfer configuration. */
cy_stc_scb_i2c_master_xfer_config_t masterTransferCfg =
{
    .slaveAddress = CODEC_I2C_ADDR,
    .buffer = NULL,
    .bufferSize = 0u,
    .xferPending = false
};

/*******************************************************************************
* Function Name: Codec_Init
********************************************************************************
* Summary:
*   Initializes the codec with default settings.
*
*
* Parameters:  
*	None
*
* Return:
*   uint32_t - I2C master transaction error status
*				CY_SCB_I2C_SUCCESS - Operation completed successfully                      
*				CY_SCB_I2C_MASTER_MANUAL_BUS_ERR - Bus error occurred   
*
*******************************************************************************/
uint32_t Codec_Init(void)
{
	uint32_t ret;
	
    /* Assert PDN Pin (if defined) */
    #ifdef CODEC_PDN_PORT
        Cy_GPIO_Set(CODEC_PDN_PORT, CODEC_PDN_NUM);
    #endif
    
    CyDelay(CODEC_RESET_WAIT_DELAY);
    
    /* Clear Power Managament 1 register */
    ret = Codec_SendData(CODEC_REG_PWR_MGMT1, 0x00);
    if (ret) return ret;
    
    /* Set the data alignment */
    ret = Codec_SendData(CODEC_REG_MODE_CTRL1, CODEC_MODE_CTRL1_DIF_24M_24M);
    if (ret) return ret;
    
    /* Set sample rate */
    ret = Codec_SendData(CODEC_REG_MODE_CTRL2, CODEC_DEF_SAMPLING_RATE);
    if (ret) return ret;
    
    /* Power-up VCOM */
    ret = Codec_SendData(CODEC_REG_PWR_MGMT1, CODEC_PWR_MGMT1_PMVCM);
    if (ret) return ret;
    
    /* Clear Digital Filter Mode register */
    ret = Codec_SendData(CODEC_REG_DIG_FILT_MODE, 0x00);
        
	return ret;  
}

/*******************************************************************************
* Function Name: Codec_SendData
********************************************************************************
* Summary:
*   Low level API to send data to codec over I2C.
*
*
* Parameters:  
*	regAddr - Address of the codec register to be updated
*	data - 8-bit data to be updated in the register
*
* Return:
*   uint32_t - I2C master transaction error status
*				CY_SCB_I2C_SUCCESS - Operation completed successfully                      
*				CY_SCB_I2C_MASTER_MANUAL_BUS_ERR - Bus error occurred    
*
*******************************************************************************/
uint32_t Codec_SendData(uint8_t regAddr, uint8_t data)
{
	uint32_t temp;
    uint8_t buffer[CODEC_PACKET_SIZE];
    
    buffer[0] = regAddr;
    buffer[1] = data;
    
    /* Setup transfer specific parameters */
    masterTransferCfg.buffer = buffer;
    masterTransferCfg.bufferSize = CODEC_PACKET_SIZE;
    
    /* Initiate write transaction */
    CodecI2CM_MasterWrite(&masterTransferCfg);
    
    /* Wait until master completes write transfer. */
    while (0u != (CodecI2CM_MasterGetStatus() & CY_SCB_I2C_MASTER_BUSY))
    {
    }
    
    /* Make sure that transfer was successful. */
    if ((CodecI2CM_MasterGetStatus() & CY_SCB_I2C_MASTER_ERR_EVENT) == 0)
    {
        temp = CY_SCB_I2C_SUCCESS;
    }
    else
    {
        temp = CY_SCB_I2C_MASTER_MANUAL_BUS_ERR;
    }

	return temp;
}

/*******************************************************************************
* Function Name: Codec_AdjustBothHeadphoneVolume
********************************************************************************
* Summary:
*   This function updates the volume of both the left and right channels of the
* 	headphone output.
*
*
* Parameters:  
*	volume - Steps of 0.375dB, where:
*            Minimum volume: -52.5dB (0x05)
*            Maximum volume: +36.0dB (0xF1)
*            Mute: (0x00)
*
* Return:
*   uint32_t - I2C master transaction error status
*				CY_SCB_I2C_SUCCESS - Operation completed successfully                      
*				CY_SCB_I2C_MASTER_MANUAL_BUS_ERR - Bus error occurred   
*
*******************************************************************************/

uint32_t Codec_AdjustBothHeadphoneVolume(uint8_t volume)
{
    uint32_t ret;
    
	if(volume > CODEC_HP_VOLUME_MAX)
	{
		volume = CODEC_HP_VOLUME_MAX;
	}
	
	ret = Codec_SendData(CODEC_REG_LCH_DIG_VOL, volume);
    if (ret) return ret;
    return Codec_SendData(CODEC_REG_RCH_DIG_VOL, volume);
}

/*******************************************************************************
* Function Name: Codec_Activate
********************************************************************************
* Summary:
*   Activates the codec - This function is called in conjunction with Codec_Deactivate API
*	after successful configuration update of the codec.
*
*
* Parameters:  
*	None
*
* Return:
*   uint32_t - I2C master transaction error status
*				CY_SCB_I2C_SUCCESS - Operation completed successfully                      
*				CY_SCB_I2C_MASTER_MANUAL_BUS_ERR - Bus error occurred   
*
*******************************************************************************/
uint32_t Codec_Activate(void)
{
    uint32_t ret;
    
    /* Enable Power Management DAC */
    ret = Codec_SendData(CODEC_REG_PWR_MGMT1, CODEC_PWR_MGMT1_PMDAC | CODEC_PWR_MGMT1_PMVCM);
    if (ret) return ret;
    
    /* Enable Left/Right Channels */
    ret = Codec_SendData(CODEC_REG_PWR_MGMT2, CODEC_PWR_MGMT2_PMHPL | CODEC_PWR_MGMT2_PMHPR);
    return ret;
}

/*******************************************************************************
* Function Name: Codec_Deactivate
********************************************************************************
* Summary:
*   Deactivates the CODEC - the configuration is retained, just the CODEC input/outputs are
*	disabled. The function should be called before changing any setting in the codec over I2C
*
*
* Parameters:  
*	None
*
* Return:
*   uint32_t - I2C master transaction error status
*				CY_SCB_I2C_SUCCESS - Operation completed successfully                      
*				CY_SCB_I2C_MASTER_MANUAL_BUS_ERR - Bus error occurred   
*
*******************************************************************************/
uint32_t Codec_Deactivate(void)
{
    uint32_t ret;
   
    /* Disable Left/Right Channels */
    ret = Codec_SendData(CODEC_REG_PWR_MGMT2, 0x00);
    
    /* Disable Power Management DAC */
    ret = Codec_SendData(CODEC_REG_PWR_MGMT1, CODEC_PWR_MGMT1_PMVCM);
    return ret;
}

/* [] END OF FILE */
