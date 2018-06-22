/*******************************************************************************
* File Name: `$INSTANCE_NAME`.h
* Version `$CY_MAJOR_VERSION`.`$CY_MINOR_VERSION`
*
* Description:
*  This file provides constants and parameter values and API definition for the 
*  GraphicLCDIntf Component
*
* Note:
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
********************************************************************************/

#if !defined(CY_GraphicLCDIntf_`$INSTANCE_NAME`_H)
#define CY_GraphicLCDIntf_`$INSTANCE_NAME`_H

#include "project.h"
#include "cyfitter.h"
    
#if (CYDEV_CHIP_FAMILY_USED == CYDEV_CHIP_FAMILY_PSOC6)
#include "syslib/cy_syslib.h"
#else
#include "cytypes.h"
#include "CyLib.h"
/* Check to see if required defines such as CY_PSOC5LP are available */
/* They are defined starting with cy_boot v3.0 */
#ifndef CY_PSOC5LP
    #error Component GraphicLCDIntf_v1_80 requires cy_boot v3.0 or later
#endif /* CY_PSOCLP */
#endif

/*************************************** 
*   Conditional Compilation Parameters
***************************************/

#define `$INSTANCE_NAME`_BUS_WIDTH (`$BitWidth`u)


/***************************************
*        Structure Definitions
***************************************/

#if (CY_UDB_V0)
    /* Sleep Mode API Support */
    typedef struct _`$INSTANCE_NAME`_backupStruct
    {
        uint8 lowPulseWidth;
        uint8 highPulseWidth;
    } `$INSTANCE_NAME`_BACKUP_STRUCT;
#endif /* (CY_UDB_V0) */


/***************************************
*        Function Prototypes            
***************************************/

void `$INSTANCE_NAME`_Start(void)           `=ReentrantKeil($INSTANCE_NAME . "_Start")`;
void `$INSTANCE_NAME`_Stop(void)            `=ReentrantKeil($INSTANCE_NAME . "_Stop")`;
void `$INSTANCE_NAME`_Write`$BitWidthReplacementString`(uint8 d_c, `$DataTypeReplacementString` wrData) \
                                            `=ReentrantKeil($INSTANCE_NAME . "_Write`$BitWidthReplacementString`")`;
void `$INSTANCE_NAME`_WriteM`$BitWidthReplacementString`(uint8 d_c, `$DataTypeReplacementString`* wrData, uint16 num) \
                                            `=ReentrantKeil($INSTANCE_NAME . "_WriteM`$BitWidthReplacementString`")`;
`$DataTypeReplacementString` `$INSTANCE_NAME`_Read`$BitWidthReplacementString`(uint8 d_c) \
                                            `=ReentrantKeil($INSTANCE_NAME . "_Read`$BitWidthReplacementString`")`;

void `$INSTANCE_NAME`_Sleep(void)           `=ReentrantKeil($INSTANCE_NAME . "_Sleep")`;
void `$INSTANCE_NAME`_Wakeup(void)          `=ReentrantKeil($INSTANCE_NAME . "_Wakeup")`;
void `$INSTANCE_NAME`_SaveConfig(void)      `=ReentrantKeil($INSTANCE_NAME . "_SaveConfig")`;   
void `$INSTANCE_NAME`_RestoreConfig(void)   `=ReentrantKeil($INSTANCE_NAME . "_RestoreConfig")`;
void `$INSTANCE_NAME`_Init(void)            `=ReentrantKeil($INSTANCE_NAME . "_Init")`;
void `$INSTANCE_NAME`_Enable(void)          `=ReentrantKeil($INSTANCE_NAME . "_Enable")`;

#define CYGRAPHICS_START()                  `$INSTANCE_NAME`_Start()
#define CYGRAPHICS_STOP()                   `$INSTANCE_NAME`_Stop()
#define CYGRAPHICS_WRITE(addr, data)        `$INSTANCE_NAME`_Write`$BitWidthReplacementString`(addr, data)
#define CYGRAPHICS_READ(addr)               `$INSTANCE_NAME`_Read`$BitWidthReplacementString`(addr)
#define CYGRAPHICS_WRITEM(addr, data, num)  `$INSTANCE_NAME`_WriteM`$BitWidthReplacementString`(addr, data, num)


/***************************************
*           API Constants               
***************************************/

#define `$INSTANCE_NAME`_CMD_QUEUE_FULL           (0x01u)
#define `$INSTANCE_NAME`_DATA_VALID               (0x02u)

#define `$INSTANCE_NAME`_READ_COMMAND             (0x02u)

#define `$INSTANCE_NAME`_DATA_ADDRESS             (0x01u)
#define `$INSTANCE_NAME`_COMMAND_ADDRESS          (0x00u)


/***************************************
*    Enumerated Types and Parameters    
***************************************/


/***************************************
*    Initial Parameter Constants        
***************************************/

/* Set 2 less than period because di must be sampled one cycle before
*  the end of Read Low intervals.
*/ 
#define `$INSTANCE_NAME`_READ_LOW_PULSE           (`$ReadLoPulse`u - 2u)   

/* Set one less than period */ 
#define `$INSTANCE_NAME`_READ_HIGH_PULSE          (`$ReadHiPulse`u - 1u)


/***************************************
*             Registers                 
***************************************/

#define `$INSTANCE_NAME`_STATUS_REG                     (* (reg8 *)  `$INSTANCE_NAME`_StsReg__STATUS_REG)
#define `$INSTANCE_NAME`_STATUS_PTR                     (  (reg8 *)  `$INSTANCE_NAME`_StsReg__STATUS_REG)

#define `$INSTANCE_NAME`_DIN_LSB_DATA_REG               (* (reg8 *)  `$INSTANCE_NAME`_LsbReg__STATUS_REG)
#define `$INSTANCE_NAME`_DIN_LSB_DATA_PTR               (  (reg8 *)  `$INSTANCE_NAME`_LsbReg__STATUS_REG)

#if (`$INSTANCE_NAME`_BUS_WIDTH == 8u)   /* 8-bit interface */
    #define `$INSTANCE_NAME`_CMD_FIFO_REG               (* (reg8 *)  `$INSTANCE_NAME`_GraphLcd8_Lsb__F0_REG)
    #define `$INSTANCE_NAME`_CMD_FIFO_PTR               (  (reg8 *)  `$INSTANCE_NAME`_GraphLcd8_Lsb__F0_REG)
    #define `$INSTANCE_NAME`_DATA_FIFO_REG              (* (reg8 *)  `$INSTANCE_NAME`_GraphLcd8_Lsb__F1_REG)
    #define `$INSTANCE_NAME`_DATA_FIFO_PTR              (  (reg8 *)  `$INSTANCE_NAME`_GraphLcd8_Lsb__F1_REG)    
    #define `$INSTANCE_NAME`_READ_LO_PULSE_REG          (* (reg8 *)  `$INSTANCE_NAME`_GraphLcd8_Lsb__D0_REG)
    #define `$INSTANCE_NAME`_READ_LO_PULSE_PTR          (  (reg8 *)  `$INSTANCE_NAME`_GraphLcd8_Lsb__D0_REG)
    #define `$INSTANCE_NAME`_READ_HI_PULSE_REG          (* (reg8 *)  `$INSTANCE_NAME`_GraphLcd8_Lsb__D1_REG)
    #define `$INSTANCE_NAME`_READ_HI_PULSE_PTR          (  (reg8 *)  `$INSTANCE_NAME`_GraphLcd8_Lsb__D1_REG)
#else   /* 16-bit interface */
    /*#if (CYDEV_CHIP_FAMILY_USED == CYDEV_CHIP_FAMILY_PSOC3) || (CYDEV_CHIP_FAMILY_USED == CYDEV_CHIP_FAMILY_PSOC5)
        #define `$INSTANCE_NAME`_DATA_FIFO_PTR              (  (reg16 *) `$INSTANCE_NAME`_GraphLcd16_Lsb__F1_REG)     
        #define `$INSTANCE_NAME`_DATA_FIFO_PTR              (  (reg16 *) `$INSTANCE_NAME`_GraphLcd16_Lsb__16BIT_F1_REG)     
    #else*/
    #define `$INSTANCE_NAME`_DATA_FIFO_PTR              (  (reg16 *) `$INSTANCE_NAME`_GraphLcd16_Lsb__16BIT_F1_REG)     
    //#endif
    #define `$INSTANCE_NAME`_CMD_FIFO_REG               (* (reg8 *)  `$INSTANCE_NAME`_GraphLcd16_Lsb__F0_REG)
    #define `$INSTANCE_NAME`_CMD_FIFO_PTR               (  (reg8 *)  `$INSTANCE_NAME`_GraphLcd16_Lsb__F0_REG)
    #define `$INSTANCE_NAME`_DIN_MSB_DATA_REG           (* (reg8 *)  `$INSTANCE_NAME`_GraphLcd16_MsbReg__STATUS_REG)
    #define `$INSTANCE_NAME`_DIN_MSB_DATA_PTR           (  (reg8 *)  `$INSTANCE_NAME`_GraphLcd16_MsbReg__STATUS_REG)
    #define `$INSTANCE_NAME`_READ_LO_PULSE_REG          (* (reg8 *)  `$INSTANCE_NAME`_GraphLcd16_Lsb__D0_REG)
    #define `$INSTANCE_NAME`_READ_LO_PULSE_PTR          (  (reg8 *)  `$INSTANCE_NAME`_GraphLcd16_Lsb__D0_REG)
    #define `$INSTANCE_NAME`_READ_HI_PULSE_REG          (* (reg8 *)  `$INSTANCE_NAME`_GraphLcd16_Lsb__D1_REG)
    #define `$INSTANCE_NAME`_READ_HI_PULSE_PTR          (  (reg8 *)  `$INSTANCE_NAME`_GraphLcd16_Lsb__D1_REG)        
#endif /* `$INSTANCE_NAME`_BUS_WIDTH == 8u */

#endif /* CY_GraphicLCDIntf_`$INSTANCE_NAME`_H */


/* [] END OF FILE */
