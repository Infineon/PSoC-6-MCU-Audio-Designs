/*****************************************************************************
* File Name: main_cm4.c
*
* Description:  This file contains the implementation of the main function and
* the I2S interrupt handler.
*
* Related Document: Code example CE218636
*
* Hardware Dependency: See code example CE218636
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
#include "project.h"
#include "codec.h"
#include "wave.h"

/* Global Variables for the I2S */
uint32_t uI2sCount = 0;

/*******************************************************************************
* Function Name: I2S_isr_Handler
****************************************************************************//**
*
* I2S Interrupt Handler Implementation. Feed the I2S internal FIFO with audio
* data.
*  
*******************************************************************************/
void I2S_isr_Handler(void)
{   
    /* Write data for the left side */
    I2S_WriteTxData((uint16) (waveData[uI2sCount]));
    
    /* Write data for the right side */
    I2S_WriteTxData((uint16) (waveData[uI2sCount]));
            
    /* If reach the end of the wave, stop the ISR */
    if (uI2sCount < NUM_ELEMENTS)
    {
       uI2sCount++; 
    }   
    else
    {
        /* Disable the ISR */
        NVIC_DisableIRQ(I2S_isr_cfg.intrSrc);        
    }

    /* Clear I2S Interrupt */
    I2S_ClearInterrupt(I2S_INTR_TX_TRIGGER_Msk);
}

/*******************************************************************************
* Function Name: main
****************************************************************************//**
*
* The main function for the Cortex-M4 CPU does the following:
*  Initialization:
*  - Initializes all the hardware blocks
*  Do forever loop:
*  - Check if the SW2 button was pressed. If yes, plays the audio stream. 
*  
*******************************************************************************/
int main(void)
{
    /* Initialize the I2S interrupt */
    Cy_SysInt_Init(&I2S_isr_cfg, I2S_isr_Handler);
    NVIC_EnableIRQ(I2S_isr_cfg.intrSrc);
 
    /* Enable global interrupts. */
    __enable_irq();
        
    /* Wait for the MCLK to clock the Audio Codec */
    CyDelay(1);
    
    /* Start I2C Master */
    CodecI2CM_Start();	
       
    /* Start the Codec */
	Codec_Init();    
    Codec_Activate();
    
    /* Start the I2S interface */
    I2S_Start();
        
    for(;;)
    {
        /* Check if the button was pressed */
        if (Cy_GPIO_Read(SW2_PORT, SW2_NUM) == 0)
        {
            /* Check if the I2S ISR is disabled */
            if (NVIC_GetEnableIRQ(I2S_isr_cfg.intrSrc))
            {
                /* I2S is enabled, do not do anything */
            }
            else /* I2S is disabled, re-start the wave buffer */
            {
                /* Restart the sound counter */
                uI2sCount = 0;
                
                /* Re-enabled the I2S ISR */
                NVIC_EnableIRQ(I2S_isr_cfg.intrSrc);
            }
        }
    }
}

/* [] END OF FILE */
