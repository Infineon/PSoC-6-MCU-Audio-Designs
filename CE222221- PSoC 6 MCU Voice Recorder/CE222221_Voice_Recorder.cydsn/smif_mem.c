/******************************************************************************
* File Name: smif_mem.c
*
* Version: 1.0
*
* Description: Functions in this file implement routines to access SMIF memory
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

#include "smif_mem.h"
#include "stdio.h"
#include "project.h"
#include "rtos.h"

/* SMIF interrupt function */
void SMIF_Interrupt_User(void);

/*******************************************************************************
* Function Name: InitMemory
********************************************************************************
* Summary:
*   This function initializes the SMIF interface.
*
*******************************************************************************/
void InitMemory(void)
{
    cy_en_smif_status_t smif_status;
    
    /* Configure SMIF interrupt */
    cy_stc_sysint_t smifIntConfig =
    {
        .intrSrc = smif_interrupt_IRQn,     /* SMIF interrupt */
        .intrPriority = SMIF_PRIORITY       /* SMIF interrupt priority */
    };    
    Cy_SysInt_Init(&smifIntConfig, SMIF_Interrupt_User);
    
    /* Init the SMIF block */
    smif_status = Cy_SMIF_Init(SMIF_1_HW, &SMIF_1_config, TIMEOUT_1_MS, &SMIF_1_context);
    if(smif_status!=CY_SMIF_SUCCESS)
    {
        HandleErrorMemory();
    }   
    Cy_SMIF_SetDataSelect(SMIF_1_HW, CY_SMIF_SLAVE_SELECT_0, CY_SMIF_DATA_SEL0);
    Cy_SMIF_Enable(SMIF_1_HW, &SMIF_1_context);  
    
    /* Enable the SMIF interrupt */
    NVIC_EnableIRQ(smif_interrupt_IRQn);
}

/*******************************************************************************
* Function Name: EraseMemory
********************************************************************************
* Summary:
*   This function erases an entire sector.
*
* Parameters:
*   sector: The sector to be erased
*
*******************************************************************************/
void EraseMemory(uint32_t sector)
{
    cy_en_smif_status_t smif_status;
    uint8_t arrayAddress[ADDRESS_SIZE] = {0};
    
    arrayAddress[0] = sector * SECTOR_MULTIPLIER;
    
    smif_status = Cy_SMIF_Memslot_CmdWriteEnable(SMIF_1_HW, smifMemConfigs[0], &SMIF_1_context);
    if(smif_status!=CY_SMIF_SUCCESS)
    {
        HandleErrorMemory();
    }
    
    smif_status = Cy_SMIF_Memslot_CmdSectorErase(SMIF_1_HW, (cy_stc_smif_mem_config_t*) smifMemConfigs[0], arrayAddress, &SMIF_1_context);
    if(smif_status!=CY_SMIF_SUCCESS)
    {
        HandleErrorMemory();
    }
    
    while(Cy_SMIF_Memslot_IsBusy(SMIF_1_HW, (cy_stc_smif_mem_config_t*)smifMemConfigs[0], &SMIF_1_context))
    {
        /* Wait till the memory controller command is completed */
        MEM_DELAY_FUNC;
    }
}

/*******************************************************************************
* Function Name: WriteMemory
********************************************************************************
* Summary:
*   This function writes data to the external memory in the quad mode. 
*   The function sends the Quad Page Program: 0x38 command to the external memory. 
*
* Parameters:
*   txBuffer: Data to write in the external memory.
*   txSize: The size of data.
*   address: The address to write data to.   
*
*******************************************************************************/
void WriteMemory(uint8_t txBuffer[], 
                    uint32_t txSize, 
                    uint32_t address)
{
    cy_en_smif_status_t smif_status;
    uint8_t arrayAddress[ADDRESS_SIZE];

    /* Convert 32-bit address to 3-byte array */
    arrayAddress[0] = CY_LO8(address >> 16);
    arrayAddress[1] = CY_LO8(address >> 8);
    arrayAddress[2] = CY_LO8(address);
    
    /* Set QE */    
    smif_status = Cy_SMIF_Memslot_QuadEnable(SMIF_1_HW, (cy_stc_smif_mem_config_t*)smifMemConfigs[0], &SMIF_1_context);
    if(smif_status!=CY_SMIF_SUCCESS)
    {
        HandleErrorMemory();
    }
    
    while(Cy_SMIF_Memslot_IsBusy(SMIF_1_HW, (cy_stc_smif_mem_config_t*)smifMemConfigs[0], &SMIF_1_context))
    {
        /* Wait till the memory controller command is completed */
        MEM_DELAY_FUNC;
    }
	
    /* Send Write Enable to external memory */	
    smif_status = Cy_SMIF_Memslot_CmdWriteEnable(SMIF_1_HW, smifMemConfigs[0], &SMIF_1_context);
    if(smif_status!=CY_SMIF_SUCCESS)
    {
        HandleErrorMemory();
    }
	
	/* Quad Page Program command */       
    smif_status = Cy_SMIF_Memslot_CmdProgram(SMIF_1_HW, smifMemConfigs[0], arrayAddress, txBuffer, txSize, &RxCmpltMemoryCallback, &SMIF_1_context);
    if(smif_status!=CY_SMIF_SUCCESS)
    {
        HandleErrorMemory();
    }	
        
    while(Cy_SMIF_Memslot_IsBusy(SMIF_1_HW, (cy_stc_smif_mem_config_t*)smifMemConfigs[0], &SMIF_1_context))
    {
        /* Wait till the memory controller command is completed */
        MEM_DELAY_FUNC;
    }
}

/*******************************************************************************
* Function Name: ReadMemory
****************************************************************************//**
* Summary:
*   This function reads data from the external memory in the quad mode. 
*   The function sends the Quad I/O Read: 0xEB command to the external memory. 
*
* Parameters:
*   rxBuffer: The buffer for read data.
*   rxSize: The size of data to read.
*   address: The address to read data from. 
*
*******************************************************************************/
void ReadMemory(uint8_t rxBuffer[], 
                            uint32_t rxSize, 
                            uint32_t address)
{   
    cy_en_smif_status_t smif_status;
    uint8_t arrayAddress[ADDRESS_SIZE];

    /* Convert 32-bit address to 3-byte array */
    arrayAddress[0] = CY_LO8(address >> 16);
    arrayAddress[1] = CY_LO8(address >> 8);
    arrayAddress[2] = CY_LO8(address);
    
    /* Set QE */		    
    smif_status = Cy_SMIF_Memslot_QuadEnable(SMIF_1_HW, (cy_stc_smif_mem_config_t*)smifMemConfigs[0], &SMIF_1_context);
    if(smif_status!=CY_SMIF_SUCCESS)
    {
        HandleErrorMemory();
    }
    
    while(Cy_SMIF_Memslot_IsBusy(SMIF_1_HW, (cy_stc_smif_mem_config_t*)smifMemConfigs[0], &SMIF_1_context))
    {
        /* Wait till the memory controller command is completed */
        MEM_DELAY_FUNC;
    }
	
	/* The 4 Page program command */    
    smif_status = Cy_SMIF_Memslot_CmdRead(SMIF_1_HW, smifMemConfigs[0], arrayAddress, rxBuffer, rxSize, &RxCmpltMemoryCallback, &SMIF_1_context);
    if(smif_status!=CY_SMIF_SUCCESS)
    {
        HandleErrorMemory();
    }
    
    while(Cy_SMIF_BusyCheck(SMIF_1_HW))
    {
        /* Wait until the SMIF IP operation is completed. */
        MEM_DELAY_FUNC;
    }
}

/*******************************************************************************
* Function Name: handle_error
********************************************************************************
* Summary:
*   This function processes unrecoverable errors such as UART component 
*   initialization error or SMIF initialization error etc. In case of such error 
*   the system will Turn on RED_LED_ERROR and stay in a infinite loop of 
*   this function.
*
*******************************************************************************/
void HandleErrorMemory(void)
{
     /* Disable all interrupts */
    __disable_irq();
	
    /* Turn on error LED */
    Cy_GPIO_Write(RED_LED_PORT, RED_LED_NUM, LED_ON);
    
    /* Halt here */
    while(1u) {}
}

/*******************************************************************************
* Function Name: SMIF_Interrupt_User
****************************************************************************//**
* Summary:
*   The ISR for the SMIF interrupt. All Read/Write transfers to/from the external 
*   memory are processed inside the SMIF ISR.
*  
*******************************************************************************/
void SMIF_Interrupt_User(void)
{
    Cy_SMIF_Interrupt(SMIF_1_HW, &SMIF_1_context);
}

/*******************************************************************************
* Function Name: RxCmpltCallback
****************************************************************************//**
* Summary:
*   The callback called after the transfer completion.
*
* Parameters:
*   event: The event of the callback.
*  
*******************************************************************************/
void RxCmpltMemoryCallback (uint32_t event)
{
    if(0u == event)
    {
        /*The process event is 0*/
    }
}

/* [] END OF FILE */

