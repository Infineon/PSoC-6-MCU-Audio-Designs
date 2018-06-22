/*****************************************************************************
* File Name: main_cm4.c
*
* Description:  This file contains the implementation of the main function and
* the PCM interrupt handler.
*
* Related Document: Code example CE219431
*
* Hardware Dependency: See code example CE219431
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
#include <stdio.h>
#include <stdlib.h>

/*******************************************************************************
* Constants 
*******************************************************************************/
/* Size of the UART string */
#define UART_STRING_SIZE            32u

/* Size of the Sample Window for the Volume */
#define SAMPLE_SIZE                 (4*PDM_PCM_RX_FIFO_TRG_LVL)

/* Noise threshold hysteresis */
#define THRESHOLD_HYSTERESIS        1u

/*******************************************************************************
* Global Variables 
*******************************************************************************/
/* PCM flag to inform that there is enough data in the FIFO to be processed */
bool flag = 0;

/* Variable to store the volume captured by the microphone */
volatile uint32_t volume;

/* Sum variable to calculate the volume */
uint32_t sumVolume = 0u;

/* Number of samples processed */
uint32_t numSamples = 0u;

/* Noise threshold */
uint32_t noiseThreshold = 0u;

/* Buffer to store the string to be sent over UART */
char uartString[UART_STRING_SIZE];

/*******************************************************************************
* Function Name: PCM_ISR_Handler
****************************************************************************//**
*
* PCM Interrupt Handler Implementation. Reads the PCM FIFO and sum its absolute
* values. 
*  
*******************************************************************************/
void PCM_ISR_Handler(void)
{             
    /* Set the PCM flag */
    flag = 1;  
    
    /* Disable PCM ISR to avoid multiple calls to this ISR */
    NVIC_DisableIRQ(PCM_ISR_cfg.intrSrc);
}

/*******************************************************************************
* Function Name: main
****************************************************************************//**
*
* The main function for the Cortex-M4 CPU does the following:
*  Initialization:
*  - Initializes all the hardware blocks
*  Do forever loop:
*  - Check if PCM flag is set. If yes, report the current volume
*  - Update the LED status based on the volume and the noise threshold
*  - Check if the SW2 is pressed. If yes, reset the noise threshold
*  
*******************************************************************************/
int main(void)
{     
    uint32_t index;
    
    /* Enable global interrupts. */
    __enable_irq(); 

    /* Initialize the PCM interrupt and enable it */
    Cy_SysInt_Init(&PCM_ISR_cfg, PCM_ISR_Handler);
    NVIC_EnableIRQ(PCM_ISR_cfg.intrSrc);

    /* Start the UART component for reporting the volume */
    UART_Start();
    
    /* Star the PCM component */
    Cy_PDM_PCM_Init(PDM_PCM_HW, &PDM_PCM_config);
    Cy_PDM_PCM_Enable(PDM_PCM_HW);
       
    for(;;)
    {
        /* Check PCM flag to process data */
        if (flag)
        {
            /* Clear the PCM flag */
            flag = 0;
             
            /* Calculate the volume by summing the absolute value of all the data
             * stored in the PCM FIFO */
            for (index = 0; index < PDM_PCM_RX_FIFO_TRG_LVL; index++)
            {
                sumVolume += abs((int32_t) PDM_PCM_ReadFifo());
            }
            
            /* Add to the number of samples */
            numSamples += PDM_PCM_RX_FIFO_TRG_LVL;
            
            /* Clear the PCM interrupt */
            PDM_PCM_ClearInterrupt(PDM_INTR_MASK_RX_TRIGGER_Msk);  
            
            /* Re-enable PCM ISR */
            NVIC_EnableIRQ(PCM_ISR_cfg.intrSrc);
            
            /* Check if reached the sample size */
            if (numSamples >= SAMPLE_SIZE)
            {
                /* Calculate the volume based on sample size */
                volume = sumVolume/(SAMPLE_SIZE);
                
                /* Reset the number of samples and the volume sum */
                numSamples = 0;
                sumVolume = 0;
                     
                /* Prepare line to report the volvume */
                UART_PutString("\n\r");
                           
                /* Report the volume over UART */
                for (index = 0; index < volume; index++)
                {
                    UART_Put('-');
                }
                   
                /* Turn ON the LED when the volume is higher than the threshold */
                if (volume > noiseThreshold)
                {
                    Cy_GPIO_Clr(LED_PORT, LED_NUM);
                }
                else
                {
                    Cy_GPIO_Set(LED_PORT, LED_NUM);
                }               
            }
        }
        
        /* Reset the noise threshold if SW2 was pressed */
        if (Cy_GPIO_GetInterruptStatus(SW2_PORT, SW2_NUM))
        {            
            /* Get the current volume and add one unit as the new noise threshold */
            noiseThreshold = volume + THRESHOLD_HYSTERESIS;
            
            /* Prepare string to report the new noise threshold */
            sprintf(uartString, "Noise Threshold: %u\n\r", (unsigned int) noiseThreshold);
            
            /* Report the new noise threshold over UART */
            UART_PutString(uartString);
            
            /* Clear SW2 interrupt */
            Cy_GPIO_ClearInterrupt(SW2_PORT, SW2_NUM);
        }
    }
}

/* [] END OF FILE */
