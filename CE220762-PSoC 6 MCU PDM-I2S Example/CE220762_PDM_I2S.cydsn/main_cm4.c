/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include "project.h"
#include "codec.h"

/* Size of the recorded buffer */
#define BUFFER_SIZE 65536

/* Array containing the recorded data */
int16_t recorded_data[BUFFER_SIZE];

/* Number of elements recorded by the PDM/PCM */
uint32_t pdm_count = 0;

int main(void)
{      
    __enable_irq(); /* Enable global interrupts. */
    
    /* Start I2C Master */
    CodecI2CM_Start();	
       
    /* Start the Codec */
	Codec_Init();    
    Codec_Activate();
    
    /* Start the I2S interface */
    I2S_Start();
    
    /* Initialize the DMAS and their descriptor addresses */
    /* PDM FIFO -> SRAM (1x) -> I2S FIFO (2x) */
    DMA_Record_Init();
    Cy_DMA_Descriptor_SetYloopDataCount(&DMA_Record_PDM_to_SRAM, BUFFER_SIZE/256); 
    Cy_DMA_Descriptor_SetSrcAddress(&DMA_Record_PDM_to_SRAM, (void *) &PDM_PCM_HW->RX_FIFO_RD);
    Cy_DMA_Descriptor_SetDstAddress(&DMA_Record_PDM_to_SRAM, (void *) &recorded_data[0]);
            
    DMA_PlayLeft_Init();
    Cy_DMA_Descriptor_SetYloopDataCount(&DMA_PlayLeft_SRAM_to_I2S, BUFFER_SIZE/256); 
    Cy_DMA_Descriptor_SetSrcAddress(&DMA_PlayLeft_SRAM_to_I2S, (void *) &recorded_data[0]);
    Cy_DMA_Descriptor_SetDstAddress(&DMA_PlayLeft_SRAM_to_I2S, (void *) &I2S_HW->TX_FIFO_WR);

    DMA_PlayRight_Init();
    Cy_DMA_Descriptor_SetYloopDataCount(&DMA_PlayRight_SRAM_to_I2S, BUFFER_SIZE/256); 
    Cy_DMA_Descriptor_SetSrcAddress(&DMA_PlayRight_SRAM_to_I2S, (void *) &recorded_data[0]);
    Cy_DMA_Descriptor_SetDstAddress(&DMA_PlayRight_SRAM_to_I2S, (void *) &I2S_HW->TX_FIFO_WR);
     
    /* Start the PCM component */
    Cy_PDM_PCM_Init(PDM_PCM_HW, &PDM_PCM_config);
    Cy_PDM_PCM_Enable(PDM_PCM_HW);

    for(;;)
    {             
        /* Check if the button was pressed or released */
        if (Cy_GPIO_GetInterruptStatus(SW2_PORT, SW2_NUM))
        {
            /* Check if the button is pressed */
            if (Cy_GPIO_Read(SW2_PORT, SW2_NUM) == 0)
            {
                /* Enable DMA to record from the microphone */
                Cy_DMA_Channel_Enable(DMA_Record_HW, DMA_Record_DW_CHANNEL);
            }
            else
            {
                /* On released, disable the record DMA */
                Cy_DMA_Channel_Disable(DMA_Record_HW, DMA_Record_DW_CHANNEL);
                
                /* Extract the number of elements recorded */
                pdm_count = DMA_Record_HW->CH_STRUCT[DMA_Record_DW_CHANNEL].CH_IDX;
                
                /* Reset the channel index for the next recording */
                DMA_Record_HW->CH_STRUCT[DMA_Record_DW_CHANNEL].CH_IDX = 0;
                
                /* If 0, it means the maximum number of bytes were transfered */
                if (pdm_count == 0)
                {
                    /* Set to the buffer size */
                    pdm_count = BUFFER_SIZE;
                }
                
                /* Setup the DMAs to play the recorded data */
                Cy_DMA_Descriptor_SetYloopDataCount(&DMA_PlayLeft_SRAM_to_I2S, pdm_count/256); 
                Cy_DMA_Descriptor_SetYloopDataCount(&DMA_PlayRight_SRAM_to_I2S, pdm_count/256);

                /* Start playing the recorded data by enabling the DMAs */
                Cy_DMA_Channel_Enable(DMA_PlayRight_HW, DMA_PlayRight_DW_CHANNEL);
                Cy_DMA_Channel_Enable(DMA_PlayLeft_HW, DMA_PlayLeft_DW_CHANNEL);                   

            }
            
            /* Clear SW2 interrupt */
            Cy_GPIO_ClearInterrupt(SW2_PORT, SW2_NUM);
        } 
    }
}

/* [] END OF FILE */
