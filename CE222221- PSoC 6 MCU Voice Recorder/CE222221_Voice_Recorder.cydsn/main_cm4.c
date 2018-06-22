/*****************************************************************************
* File Name: main_cm4.c
*
* Version: 1.0
*
* Description: Main function implementation for CM4.
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
#include "project.h"
#include "codec.h"
#include "cy_smif_memconfig.h"
#include "smif_mem.h"
#include "touch.h"
#include "recorder.h"
#include "events.h"
#include "rtos.h"
#include "graphics.h"

/* RTOS Queue Variables */
QueueHandle_t EventsQueue;
QueueHandle_t GUIQueue;

/* RTOS Semaphore/Mutex Variables */
SemaphoreHandle_t SmifSemphr;

/* RTOS Event Group bits */
EventGroupHandle_t DmaEvents;

/* RTOS Timer */
TimerHandle_t SoftTimer;

/* RTOS Constants */
#define CORTEX_INTERRUPT_BASE               ( 16 )
#define STACK_DEPTH                         (400u)

/* Declarations of the exception handlers for FreeRTOS */
extern void vPortSVCHandler(void);
extern void xPortPendSVHandler(void);
extern void xPortSysTickHandler(void);
void FreeRTOSSetup(void);

/*******************************************************************************
* Function Name: main()
********************************************************************************
* Summary:
*   Main function of Cortex-M4. Initializes the hardware, create all RTOS 
*   related elements and run the RTOS scheduler.
*
*******************************************************************************/
int main(void)
{          
    __enable_irq(); /* Enable global interrupts. */   
               
    /* Start I2C Master */
    CodecI2CM_Start();	
        
    /* Start touch */
    InitTouch();

    /* Start external memory */
    InitMemory();    
    
    /* Create Queues */
    EventsQueue = xQueueCreate(QUEUE_SIZE, sizeof(uint32_t));
    GUIQueue = xQueueCreate(QUEUE_SIZE, sizeof(uint32_t));
    
    /* Create Semaphores */
    SmifSemphr = xSemaphoreCreateBinary();
    
    /* Create Event Group Bits */
    DmaEvents = xEventGroupCreate();
    
    /* Create Tasks */
    xTaskCreate(RecorderTask, "Recorder Task", STACK_DEPTH, NULL, 1, NULL);
    xTaskCreate(TouchTask, "Touch Task", STACK_DEPTH, NULL, 1, NULL);
    xTaskCreate(GraphicsTask, "Graphics Task", STACK_DEPTH, NULL, 1, NULL);
    xTaskCreate(EventsTask, "Command Task", STACK_DEPTH, NULL, 1, NULL);
    
    vTaskStartScheduler(); 
    
    for(;;)
    {             

    }
}

/*******************************************************************************
* Function Name: FreeRTOSSetup()
********************************************************************************
* Summary:
*   Sets up required exception handlers.
*
*******************************************************************************/
void FreeRTOSSetup( void )
{
    /* Handler for Cortex Supervisor Call (SVC, formerly SWI) - address 11 */
    __ramVectors[CORTEX_INTERRUPT_BASE + SVCall_IRQn] = (cy_israddress)vPortSVCHandler;

    /* Handler for Cortex PendSV Call - address 14 */
    __ramVectors[CORTEX_INTERRUPT_BASE + PendSV_IRQn] = (cy_israddress)xPortPendSVHandler;

    /* Handler for Cortex SYSTICK - address 15 */
    __ramVectors[CORTEX_INTERRUPT_BASE + SysTick_IRQn] = (cy_israddress)xPortSysTickHandler;
}

/* [] END OF FILE */
