/******************************************************************************
* File Name: recorder.c
*
* Version: 1.0
*
* Description: This file contains the functions used to initialize and handle
*              recordings
*
* Related Document: N/A
*
* Hardware Dependency: CY8CKIT-062-WiFi-BT PSoC 6 WiFi-BT Pioneer Kit
*
******************************************************************************
* Copyright (2018), Cypress Semiconductor Corporation.
******************************************************************************
* This software, including source code, documentation and related materials
* ("Software") is owned by Cypress Semiconductor Corporation (Cypress) and is
* protected by and subject to worldwide patent protection (United States and
* foreign), United States copyright laws and international treaty provisions.
* Cypress hereby grants to licensee a personal, non-exclusive, non-transferable
* license to copy, use, modify, create derivative works of, and compile the
* Cypress source code and derivative works for the sole purpose of creating
* custom software in support of licensee product, such licensee product to be
* used only in conjunction with Cypress's integrated circuit as specified in the
* applicable agreement. Any reproduction, modification, translation, compilation,
* or representation of this Software except as specified above is prohibited
* without the express written permission of Cypress.
*
* Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
* Cypress reserves the right to make changes to the Software without notice.
* Cypress does not assume any liability arising out of the application or use
* of Software or any product or circuit described in the Software. Cypress does
* not authorize its products for use as critical components in any products
* where a malfunction or failure may reasonably be expected to result in
* significant injury or death ("ACTIVE Risk Product"). By including Cypress's
* product in a ACTIVE Risk Product, the manufacturer of such system or application
* assumes all risk of such use and in doing so indemnifies Cypress against all
* liability. Use of this Software may be limited by and subject to the applicable
* Cypress software license agreement.
*****************************************************************************/
#include "recorder.h"
#include "project.h"
#include "smif_mem.h"
#include "graphics.h"
#include "rtos.h"

/*******************************************************************************
*            Local Interrupt Handlers
*******************************************************************************/
void PDM_Interrupt_User(void);
void I2S_Interrupt_User(void);

/*******************************************************************************
*            Internal Global Variables
*******************************************************************************/
uint32_t pageExCount = 0;                   /* Current page access to SMIF */
uint32_t pageTxCount = 0;                   /* Current page to TX buffer */
uint32_t pageRxCount = 0;                   /* Current page to RX buffer */
uint8_t txBuffer[PACKET_SIZE*TX_PAGE_MAX_COUNT] = {0};     
                                            /* TX buffer from PDM to SMIF */
uint8_t rxBuffer[PACKET_SIZE*2] = {0};      /* RX buffer from SMIF to I2S */
sector_info_t sectorInfo;                   /* Information of the current sector */
recorder_states_t state = IDLE;             /* Current state */
uint32_t startSectorRecorded = 0;           /* Start sector of the last record */
uint32_t endSectorRecorded = 0;             /* Last sector of the last recorded */
uint32_t currentInfoAddress;                /* Current address storing the index */
uint32_t currentSector;                     /* Current sector being accessed */

/*******************************************************************************
* Function Name: InitRecorder
********************************************************************************
* Summary:
*   This function initializes all the blocks related to recording, including PDM
*   and I2S.
*
*******************************************************************************/
void InitRecorder(void)
{
    uint32_t index;
    uint32_t memAddress = 0;
    
    /* Init Local Interrupts */
    Cy_SysInt_Init(&DMA_PDM_IRQ_cfg, PDM_Interrupt_User);
    NVIC_EnableIRQ(DMA_PDM_IRQ_cfg.intrSrc);
    Cy_SysInt_Init(&DMA_I2S_IRQ_cfg, I2S_Interrupt_User);
    NVIC_EnableIRQ(DMA_I2S_IRQ_cfg.intrSrc);
    
    /* Initialize the hardware blocks */
    Cy_I2S_Init(I2S_HW, &I2S_config);
    Cy_PDM_PCM_Init(PDM_PCM_HW, &PDM_PCM_config);
    
    /* Initialize the DMAS and their descriptor addresses */
    DMA_Record_Init();
    Cy_DMA_Descriptor_SetSrcAddress(&DMA_Record_PDM_to_SRAM, (void *) &PDM_PCM_HW->RX_FIFO_RD);
    Cy_DMA_Descriptor_SetDstAddress(&DMA_Record_PDM_to_SRAM, (void *) &txBuffer[0]);
    DMA_Record_SetInterruptMask(DMA_Record_INTR_MASK);
            
    DMA_PlayLeft_Init();
    Cy_DMA_Descriptor_SetSrcAddress(&DMA_PlayLeft_SRAM_to_I2S, (void *) &rxBuffer[0]);
    Cy_DMA_Descriptor_SetDstAddress(&DMA_PlayLeft_SRAM_to_I2S, (void *) &I2S_HW->TX_FIFO_WR);

    DMA_PlayRight_Init();
    Cy_DMA_Descriptor_SetSrcAddress(&DMA_PlayRight_SRAM_to_I2S, (void *) &rxBuffer[0]);
    Cy_DMA_Descriptor_SetDstAddress(&DMA_PlayRight_SRAM_to_I2S, (void *) &I2S_HW->TX_FIFO_WR);
    DMA_PlayRight_SetInterruptMask(DMA_PlayRight_INTR_MASK);    
    
    /* Scan the SMIF memory sector ONE to obtain information of where to record/play */
    /* Check if has anything in the memory */
    ReadMemory((uint8_t *) &sectorInfo, PACKET_SIZE, 0);
        
    /* Check if signature was set */
    if (sectorInfo.signature == SIGNATURE)
    {       
        /* Scan for the latest page available in the info sector */
        for (index = 1; index < NUM_PAGES_IN_SECTOR; index++)
        {     
            /* Read information from the other pages in the info sector */
            memAddress = index * PACKET_SIZE;
            ReadMemory((uint8_t *) &sectorInfo, PACKET_SIZE, memAddress);
            
            /* Check if signature is presented */
            if (sectorInfo.signature == SIGNATURE)
            {                      
                /* Signature match, keep searching */
            }
            else
            {
                /* No signature presented, found last page available */
                
                /* Recover the previous information */
                memAddress = (index-1) * PACKET_SIZE;
                ReadMemory((uint8_t *) &sectorInfo, PACKET_SIZE, memAddress);              
                break;
            }
        }
        
        /* If no avaliable pages, erase the entire memory */
        if (index == NUM_PAGES_IN_SECTOR)
        {
            EraseMemory(INFO_SECTOR);
            
            /* Write new signature */
            /* Write a new signature */
            sectorInfo.signature = SIGNATURE;
            sectorInfo.currentSector = 1;
            sectorInfo.numberOfPagesRecorded = 0;
            
            /* Write new info */
            WriteMemory((uint8_t *) &sectorInfo, PACKET_SIZE, 0);
            
            /* Update current address storing the info sector */
            currentInfoAddress = 0;
        }
        else
        {
            /* Update current info address */
            memAddress = (index-1) * PACKET_SIZE;
            /* Update current address storing the info sector */
            currentInfoAddress = memAddress;
            
        }
        
    }
    else /* If no signature, erase the memory */
    {
        EraseMemory(INFO_SECTOR);
        
        /* Write a new signature */
        sectorInfo.signature = SIGNATURE;
        sectorInfo.currentSector = 1;
        sectorInfo.numberOfPagesRecorded = 0;
        
        /* Write new info */
        WriteMemory((uint8_t *) &sectorInfo, PACKET_SIZE, 0);
        
        /* Update current address storing the info sector */
        currentInfoAddress = 0;
    }
    
    /* Update start/end sector variables */
    startSectorRecorded = sectorInfo.currentSector;
    endSectorRecorded = sectorInfo.currentSector + sectorInfo.numberOfPagesRecorded/NUM_PAGES_IN_SECTOR;
    
    /* Update the TX Counter */
    pageTxCount = sectorInfo.numberOfPagesRecorded;
    pageExCount = pageTxCount;
    
    /* If the end sector is higher the number of sectors in the memory, wrap it up */
    if (endSectorRecorded >= NUM_PAGES_IN_SECTOR*NUM_SECTORS_IN_MEM)
    {
        endSectorRecorded = endSectorRecorded - NUM_PAGES_IN_SECTOR*NUM_SECTORS_IN_MEM + 1;
    }
       
    /* Erase the next sector to be ready for recording */
    if (endSectorRecorded == (NUM_PAGES_IN_SECTOR*NUM_SECTORS_IN_MEM-1))
    {
        EraseMemory(FIRST_RECORD_SECTOR);
    }
    else
    {
        EraseMemory(endSectorRecorded+1);        
    }
    
    /* Enable PDM block */
    Cy_PDM_PCM_Enable(PDM_PCM_HW);
    
    /* Release SMIF semaphore */ 
    xSemaphoreGive(SmifSemphr);
}

/*******************************************************************************
* Function Name: StartRecorder
********************************************************************************
* Summary:
*   This function starts a record. It enables the DMA connected to the PDM/PCM.
*
*******************************************************************************/
void StartRecorder(void)
{       
    /* Set the start sector recorded */
    startSectorRecorded = endSectorRecorded + 1;

    if (startSectorRecorded >= NUM_SECTORS_IN_MEM)
    {
        startSectorRecorded = 1;
    }
    
    /* Update end sector variable */
    endSectorRecorded = startSectorRecorded;
           
    /* Initialize the page counters */
    pageTxCount = 0;
    pageExCount = 0;
           
    /* If playing, stop the I2S and DMAs */
    Cy_DMA_Channel_Disable(DMA_PlayRight_HW, DMA_PlayRight_DW_CHANNEL);
    Cy_DMA_Channel_Disable(DMA_PlayLeft_HW, DMA_PlayLeft_DW_CHANNEL);  
    I2S_Stop();    
    
    /* Reset the channel index for the next recording */
    DMA_Record_HW->CH_STRUCT[DMA_Record_DW_CHANNEL].CH_IDX = 0;
    
    /* Clean-up the PDM_PCM FIFO */
    Cy_PDM_PCM_ClearFifo(PDM_PCM_HW);
    
    /* Enable DMA to record from the microphone */
    Cy_DMA_Channel_Enable(DMA_Record_HW, DMA_Record_DW_CHANNEL);
        
    state = RECORDING;
}

/*******************************************************************************
* Function Name: StopRecorder
********************************************************************************
* Summary:
*   This function stops a record. It disables the DMA connected to the PDM/PCM.
*
*******************************************************************************/
void StopRecorder(void)
{
    uint32_t memAddress;
                
    /* On released, disable the record DMA */
    Cy_DMA_Channel_Disable(DMA_Record_HW, DMA_Record_DW_CHANNEL);
    
    /* Update page in the Info Sector */
    sectorInfo.signature = SIGNATURE;
    sectorInfo.currentSector = startSectorRecorded;
    sectorInfo.numberOfPagesRecorded = pageTxCount;
    
    memAddress = currentInfoAddress + PACKET_SIZE;
    
    /* Reserve the SMIF for this task */ 
    xSemaphoreTake(SmifSemphr, portMAX_DELAY);
    
    /* Check if all pages were used */
    if (memAddress >= SECTOR_SIZE)
    {
        /* Erase info sector */
        EraseMemory(INFO_SECTOR);
        
        /* Reset address */
        memAddress = 0;
    }
        
    WriteMemory((uint8_t *) &sectorInfo, PACKET_SIZE, memAddress);
    
    currentInfoAddress = memAddress;
    
    /* Erase the next sector to be recorded */
    if (endSectorRecorded == (NUM_PAGES_IN_SECTOR*NUM_SECTORS_IN_MEM-1))
    {        
        /* Erase the sector, so data can be written to it */
        EraseMemory(FIRST_RECORD_SECTOR); 
    }
    else
    {        
        /* Erase the sector, so data can be written to it */
        EraseMemory(endSectorRecorded+1); 
    }
   
    /* Release SMIF semaphore */ 
    xSemaphoreGive(SmifSemphr);
    
    state = IDLE;
}

/*******************************************************************************
* Function Name: PlayRecorder
********************************************************************************
* Summary:
*   This function plays the last record. It enables the I2S and the DMAs 
*   connected to it.
*
*******************************************************************************/
void PlayRecorder(void)
{
    uint32_t memAddress;
    
    /* Reserve the SMIF for this task */ 
    xSemaphoreTake(SmifSemphr, portMAX_DELAY);
    
    /* Fill up rxBuffer */
    memAddress = startSectorRecorded * SECTOR_SIZE;
    ReadMemory(&rxBuffer[0], PACKET_SIZE, memAddress);
        
    memAddress = startSectorRecorded * SECTOR_SIZE + PACKET_SIZE;        
    ReadMemory(&rxBuffer[PACKET_SIZE], PACKET_SIZE, memAddress);
    
    /* Release SMIF semaphore */ 
    xSemaphoreGive(SmifSemphr);
    
    pageRxCount = 2;
             
    DMA_PlayRight_HW->CH_STRUCT[DMA_PlayRight_DW_CHANNEL].CH_IDX = 0;
    DMA_PlayLeft_HW->CH_STRUCT[DMA_PlayLeft_DW_CHANNEL].CH_IDX = 0;
    
    I2S_Start();
                
    /* Start playing the recorded data by enabling the DMAs */
    Cy_DMA_Channel_Enable(DMA_PlayRight_HW, DMA_PlayRight_DW_CHANNEL);
    Cy_DMA_Channel_Enable(DMA_PlayLeft_HW, DMA_PlayLeft_DW_CHANNEL); 

    state = PLAYING;
}

/*******************************************************************************
* Function Name: RecorderTask
********************************************************************************
* Summary:
*   This function is responsible to housekeep the recording or the playing. It 
*   checks the DMA flags and write/read data to/from the external memory.
*
* Parameters:
*   arg: Required argument for task function.
*
*******************************************************************************/
void RecorderTask(void *arg)
{
    uint32_t memAddress;
    uint32_t event;
    uint32_t graphics_event;
    EventBits_t dmaBits;
    uint32_t time = 0;
    static uint32_t lastTime = 0;
    (void) arg;
    
    InitRecorder();  
    
    while (1)
    {
        dmaBits = xEventGroupWaitBits(
                        DmaEvents, 
                        DMA_I2S_FLAG_BIT | DMA_PDM_FLAG_BIT | RECORD_FLAG_BIT,
                        true,
                        false,
                        portMAX_DELAY);
        
        /* Handle writes to the External Memory in chucks */
        if (pageTxCount > pageExCount)
        {
            /* OK to keep recording */
            memAddress = (startSectorRecorded * SECTOR_SIZE) + (pageExCount * PACKET_SIZE);
            
            /* If the address is higher than the size of the memory, wrap up the address */
            if (memAddress >= (SECTOR_SIZE*NUM_SECTORS_IN_MEM))
            {
                memAddress = SECTOR_SIZE + memAddress-(SECTOR_SIZE*NUM_SECTORS_IN_MEM);
            }
            
            /* Reserve the SMIF for this task */ 
            xSemaphoreTake(SmifSemphr, portMAX_DELAY);
            
            /* Check if overlap sector */
            if ((memAddress % SECTOR_SIZE == 0) && (memAddress != (startSectorRecorded * SECTOR_SIZE)))
            {
                /* Erase the sector first */
                EraseMemory(memAddress/SECTOR_SIZE);
                
                endSectorRecorded = memAddress/SECTOR_SIZE;
            }
            
            /* Write recorded data to the FLASH */
            WriteMemory(&txBuffer[(pageExCount % TX_PAGE_MAX_COUNT)*PACKET_SIZE], PACKET_SIZE, memAddress);
            
            /* Release SMIF semaphore */ 
            xSemaphoreGive(SmifSemphr);
            
            pageExCount++;    
        }
        
        /* Handle the DMA PDM interrupt */
        if (dmaBits & DMA_PDM_FLAG_BIT)
        {                      
            /* Increment the page TX count, if the limit not reached */
            if (pageTxCount < (MAX_RECORD_SIZE*NUM_PAGES_IN_SECTOR))
            {                
                pageTxCount++;
            }
            else
            {
                /* No recording */
                state = IDLE;
            }
            
            if (state == IDLE)
            {
                StopRecorder();
                
                event = REACH_MEM_LIMIT;
                xQueueSend(EventsQueue, &event, 0);
            }
            
            xEventGroupSetBits(DmaEvents, RECORD_FLAG_BIT);
        }
        
        /* Handle the DMA I2S interrupt */
        if (dmaBits & DMA_I2S_FLAG_BIT)
        {           
            /* Reserve the SMIF for this task */ 
            xSemaphoreTake(SmifSemphr, portMAX_DELAY);
            
            /* Read next part of the memory */
            memAddress = (startSectorRecorded * SECTOR_SIZE) + (pageRxCount * PACKET_SIZE);
            ReadMemory(&rxBuffer[(pageRxCount % 2)*PACKET_SIZE], PACKET_SIZE, memAddress);
                 
            /* Release SMIF semaphore */ 
            xSemaphoreGive(SmifSemphr);
            
            pageRxCount++;
            
            if (pageRxCount < (pageTxCount) )
            {
                /* Keep playing */
            }
            else if (pageRxCount >= (pageTxCount))
            {
                Cy_DMA_Channel_Disable(DMA_PlayRight_HW, DMA_PlayRight_DW_CHANNEL);
                Cy_DMA_Channel_Disable(DMA_PlayLeft_HW, DMA_PlayLeft_DW_CHANNEL);  

                I2S_Stop();
                
                /* Play the whole track */
                state = IDLE;
                
                event = PLAY_COMPLETED;
                xQueueSend(EventsQueue, &event, 0);
            }            
            
            xEventGroupSetBits(DmaEvents, RECORD_FLAG_BIT);
        }  
        
        /* Update the timer on screen */
        if (state == RECORDING)
        {
            /* If recording, play based on pageTxCount */
            time = (pageTxCount/32);
            
        } else if (state == PLAYING)
        {
            /* If playing, show based on pageRx Count */
            time = (pageRxCount/32 );           
        }
        
        /* Only update if the time changed */
        if (time != lastTime)
        {
            graphics_event = SHOW_TIMER | (time);
            xQueueSend(GUIQueue, &graphics_event, 0);
        }
            
        lastTime = time;        
    }
}

/*******************************************************************************
* Function Name: RecorderState
********************************************************************************
* Summary:
*   Return the current state of the recording.
*
* Return:
*   recorder_states_t: current state of the recording.
*     IDLE
*     RECORDING
*     PLAYING
*     PAUSED
*
*******************************************************************************/
recorder_states_t RecorderState(void)
{
    return state;
}

/*******************************************************************************
* Function Name: PauseRecorder
********************************************************************************
* Summary:
*   Pause the recording. It only disable the DMAs connected to the I2S.
*
*******************************************************************************/
void PauseRecorder(void)
{
    state = PAUSED;
    
    Cy_DMA_Channel_Disable(DMA_PlayRight_HW, DMA_PlayRight_DW_CHANNEL);
    Cy_DMA_Channel_Disable(DMA_PlayLeft_HW, DMA_PlayLeft_DW_CHANNEL);  
}

/*******************************************************************************
* Function Name: ResumeRecorder
********************************************************************************
* Summary:
*   Resume the recording. It only enable the DMAs connected to the I2S.
*
*******************************************************************************/
void ResumeRecorder(void)
{
    state = PLAYING;
    
    Cy_DMA_Channel_Enable(DMA_PlayRight_HW, DMA_PlayRight_DW_CHANNEL);
    Cy_DMA_Channel_Enable(DMA_PlayLeft_HW, DMA_PlayLeft_DW_CHANNEL);
}
   
/*******************************************************************************
* Function Name: ResetRecorder
********************************************************************************
* Summary:
*   Stops sending data over I2S and disable DMAs.
*
*******************************************************************************/
void ResetRecorder(void)
{
    state = IDLE;
    
    Cy_DMA_Channel_Disable(DMA_PlayRight_HW, DMA_PlayRight_DW_CHANNEL);
    Cy_DMA_Channel_Disable(DMA_PlayLeft_HW, DMA_PlayLeft_DW_CHANNEL);
    
    I2S_Stop();

}

/*******************************************************************************
* Function Name: PDM_Interrupt_User
********************************************************************************
* Summary:
*   This interrupt is triggered when the PDM DMA transfer cycle is completed.
*
*******************************************************************************/
void PDM_Interrupt_User(void)
{
    BaseType_t higherPriorityTaskWoken, result;
    
    result = xEventGroupSetBitsFromISR(DmaEvents, DMA_PDM_FLAG_BIT, &higherPriorityTaskWoken);
    
    Cy_DMA_Channel_ClearInterrupt(DMA_Record_HW, DMA_Record_DW_CHANNEL);
    
    if (result != pdFAIL)
    {
        portYIELD_FROM_ISR(higherPriorityTaskWoken);
    }    
}

/*******************************************************************************
* Function Name: I2S_Interrupt_User
********************************************************************************
* Summary:
*   This interrupt is triggered when the I2S DMA transfer cycle is completed.
*
*******************************************************************************/
void I2S_Interrupt_User(void)
{
    BaseType_t higherPriorityTaskWoken, result;
    
    result = xEventGroupSetBitsFromISR(DmaEvents, DMA_I2S_FLAG_BIT, &higherPriorityTaskWoken);
    
    Cy_DMA_Channel_ClearInterrupt(DMA_PlayRight_HW, DMA_PlayRight_DW_CHANNEL);
    
    if (result != pdFAIL)
    {
        portYIELD_FROM_ISR(higherPriorityTaskWoken);
    }
}

/* [] END OF FILE */


