/*******************************************************************************
* File Name: `$INSTANCE_NAME`.c  
* Version `$CY_MAJOR_VERSION`.`$CY_MINOR_VERSION`
*
* Description:
*  This file contains the setup, control and status commands for the 
*  GraphicLCDIntf component.  
*
* Note: 
*
*******************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
********************************************************************************/

#include "`$INSTANCE_NAME`.h"  

uint8 `$INSTANCE_NAME`_initVar = 0u;


/*******************************************************************************
* Function Name: `$INSTANCE_NAME`_Enable
********************************************************************************
*
* Summary:
*  There is no need to reset or enable this component.  This component is 
*  naturally a slave that waits for the API calls in order to perform any 
*  function and once a request is made that request will always complete in 
*  finite time as long as a clock is provided. The only thing that is provided
*  by this API is setting the Command FIFO to drive at least half empty status
*  from the FIFO.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void `$INSTANCE_NAME`_Enable(void) `=ReentrantKeil($INSTANCE_NAME . "_Enable")`
{   
    /* There aren't anything to enable */
}


/*******************************************************************************
* Function Name: `$INSTANCE_NAME`_Init
********************************************************************************
*
* Summary:
*  Inits/Restores default GraphicLCDIntf configuration provided with customizer
*
* Parameters:
*  None.
*
* Return:
*  None.
* 
*******************************************************************************/
void `$INSTANCE_NAME`_Init(void) `=ReentrantKeil($INSTANCE_NAME . "_Init")`
{
    /* Set low and high pulse widths for read transaction */
    `$INSTANCE_NAME`_READ_LO_PULSE_REG = `$INSTANCE_NAME`_READ_LOW_PULSE;
    `$INSTANCE_NAME`_READ_HI_PULSE_REG = `$INSTANCE_NAME`_READ_HIGH_PULSE;
}


/*******************************************************************************
* Function Name: `$INSTANCE_NAME`_Start
********************************************************************************
*
* Summary:
*  Starts the GraphicLCDIntf interface.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  `$INSTANCE_NAME`_initVar - used to check initial configuration, modified on 
*  first function call.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void `$INSTANCE_NAME`_Start(void) `=ReentrantKeil($INSTANCE_NAME . "_Start")`
{
    if (`$INSTANCE_NAME`_initVar == 0u)
    {
        `$INSTANCE_NAME`_Init();
        `$INSTANCE_NAME`_initVar = 1u;
    }
      
    `$INSTANCE_NAME`_Enable();
}


/*******************************************************************************
* Function Name: `$INSTANCE_NAME`_Stop
********************************************************************************
*
* Summary:
*  Disables the GraphicLCDIntf interface.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void `$INSTANCE_NAME`_Stop(void) `=ReentrantKeil($INSTANCE_NAME . "_Stop")`
{
    /* There aren't anything to stop */
}


/*******************************************************************************
* Function Name: `$INSTANCE_NAME`_Write`$BitWidthReplacementString`
********************************************************************************
*
* Summary:
*  Initiates a write transaction on the `$BitWidthReplacementString`-bit parallel
*  interface.
*  The write is a posted write, so this function will return before the write 
*  has actually completed on the interface. If the command queue is full, this 
*  function will not return until space is availale to queue this write request.
*
* Parameters:
*  d_c:  Data(1) or Command(0) indication. Passed to the d_c pin.
*
*  data: Data sent on the do_msb[7:0] (most significant byte) 
*        and do_lsb[7:0] (least significant byte) pins. do_msb[7:0]
*        presents only for 16-bit interface.
*
* Return:
*  None.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void `$INSTANCE_NAME`_Write`$BitWidthReplacementString`(uint8 d_c, `$DataTypeReplacementString` wrData) \
                                                 `=ReentrantKeil($INSTANCE_NAME . "_Write`$BitWidthReplacementString`")`
{
    while((`$INSTANCE_NAME`_STATUS_REG & `$INSTANCE_NAME`_CMD_QUEUE_FULL) != 0u)
    {
            /* Command queue is full */
    }   
    `$INSTANCE_NAME`_CMD_FIFO_REG = d_c;
    #if (`$INSTANCE_NAME`_BUS_WIDTH == 16u)
        CY_SET_REG16(`$INSTANCE_NAME`_DATA_FIFO_PTR, wrData);
    #else /* 8-bit interface */
        `$INSTANCE_NAME`_DATA_FIFO_REG = wrData;
    #endif /* `$INSTANCE_NAME`_BUS_WIDTH == 16u */
}
	

/*******************************************************************************
* Function Name: `$INSTANCE_NAME`_WriteM`$BitWidthReplacementString`
********************************************************************************
*
* Summary:
*  Initiates multiple write transactions on the `$BitWidthReplacementString`-bit 
*  parallel interface. The write is a posted write, so this function will return
*  before the write has actually completed on the interface. If the command 
*  queue is full, this function will not return until space is availale to queue
*  this write request.
*
* Parameters:
*  d_c:  Data(1) or Command(0) indication. Passed to the d_c pin.
*
*  data: Pointer to the the first element of a data buffer sent on the do_msb[7:0]
*        (most significant byte) and do_lsb[7:0] (least significant byte) pins. 
*        do_msb[7:0] presents only for 16-bit interface.
*
*  num:  Number of words/bytes to write.
*
* Return:
*  None.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void `$INSTANCE_NAME`_WriteM`$BitWidthReplacementString`(uint8 d_c, `$DataTypeReplacementString`* wrData, uint16 num) \
                                                `=ReentrantKeil($INSTANCE_NAME . "_WriteM`$BitWidthReplacementString`")`
{
    for(; num; num--)
    {
        while((`$INSTANCE_NAME`_STATUS_REG & `$INSTANCE_NAME`_CMD_QUEUE_FULL) != 0u)
        {
            /* Command queue is full */
        }   
        `$INSTANCE_NAME`_CMD_FIFO_REG = d_c;  
 
        #if (`$INSTANCE_NAME`_BUS_WIDTH == 16u)
            CY_SET_REG16(`$INSTANCE_NAME`_DATA_FIFO_PTR, *wrData++);
        #else /* 8-bit interface */
            `$INSTANCE_NAME`_DATA_FIFO_REG = *wrData++;
        #endif /* `$INSTANCE_NAME`_BUS_WIDTH == 16u */
    }
}


/*******************************************************************************
* Function Name: `$INSTANCE_NAME`_Read`$BitWidthReplacementString`
********************************************************************************
*
* Summary:
*  Initiates a read transaction on the `$BitWidthReplacementString`-bit parallel interface.
*  The read will execute after all currently posted writes have completed. This 
*  function will wait until the read completes and then returns the read value.
*
* Parameters:
*  d_c: Data(1) or Command(0) indication. Passed to the d_c pin.
*
* Return:
*  16-bit read value from the di_msb[7:0] (most significant byte) and 
*  di_lsb[7:0] (least significant byte) pins for 16-bit interface or
*  8-bit read value from the di_lsb[7:0] for 8-bit interface.
*
* Reentrant:
*  No.
*
*******************************************************************************/
`$DataTypeReplacementString` `$INSTANCE_NAME`_Read`$BitWidthReplacementString`(uint8 d_c) \
                                                  `=ReentrantKeil($INSTANCE_NAME . "_Read`$BitWidthReplacementString`")`
{
    while((`$INSTANCE_NAME`_STATUS_REG & `$INSTANCE_NAME`_CMD_QUEUE_FULL) != 0u)
    {
        /* Command queue is full */
    }   
    `$INSTANCE_NAME`_CMD_FIFO_REG = d_c | `$INSTANCE_NAME`_READ_COMMAND;
    
    while((`$INSTANCE_NAME`_STATUS_REG & `$INSTANCE_NAME`_DATA_VALID) != `$INSTANCE_NAME`_DATA_VALID)
    {
        /* wait until input data are valid */    
    }
    
    #if (`$INSTANCE_NAME`_BUS_WIDTH == 8u)  /* 8-bit interface */
        return (`$INSTANCE_NAME`_DIN_LSB_DATA_REG);
    #else   /* 16-bit interface */
        return ((((uint16) `$INSTANCE_NAME`_DIN_MSB_DATA_REG) << 8u) | `$INSTANCE_NAME`_DIN_LSB_DATA_REG); 
    #endif /* `$INSTANCE_NAME`_BUS_WIDTH == 8u */
}


/* [] END OF FILE */
