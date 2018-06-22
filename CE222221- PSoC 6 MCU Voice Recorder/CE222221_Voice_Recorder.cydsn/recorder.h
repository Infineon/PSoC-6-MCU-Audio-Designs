/******************************************************************************
* File Name: recorder.h
*
* Version: 1.0
*
* Description: This file declares the functions provided by the recorder.c file
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

/* Include Guard */
#ifndef RECORDER_H
#define RECORDER_H

#include "project.h"

/*******************************************************************************
*            Structures and Enums
*******************************************************************************/
typedef struct sector_info
{
    uint32_t signature;               /* Signature to validate content on FLASH */
    uint32_t currentSector;           /* Current sector to be written */
    uint32_t numberOfPagesRecorded;   /* Number of pages recorded */
    uint32_t reserved[125];           /* For future use */
} sector_info_t;
    
/* Enumerated data type for different states of the recorder */
typedef enum
{
    IDLE            = 0x00u,
    RECORDING       = 0x01u,
    PLAYING         = 0x02u,
    PAUSED          = 0x03u,
}   recorder_states_t;

/* Enumerated data type for different types of recording events. Integer values 
   are added so that this data type can be used to as a parameter to function 
   pointers */
typedef enum
{
    NO_RECORD_EVENT = 0x00000000u,
    PLAY_COMPLETED  = 0x20000001u,
    REACH_MEM_LIMIT = 0x20000002u,
}   record_events_t;

/*******************************************************************************
*            Function Prototypes
*******************************************************************************/
void InitRecorder(void);
void StartRecorder(void);
void StopRecorder(void);
void PlayRecorder(void);
void PauseRecorder(void);
void ResumeRecorder(void);
void ResetRecorder(void);
void RecorderTask(void *arg);
recorder_states_t RecorderState(void);

/*******************************************************************************
*            Constants
*******************************************************************************/
#define SIGNATURE           (0xDEADBEEFu)   /* Signature for Sector Info */
#define MAX_RECORD_SIZE     (32u)           /* Maximum number of sectors per record */
#define FIRST_RECORD_SECTOR (1u)            /* First sector for recording */
#define INFO_SECTOR         (0u)            /* Sector reserved for info */
#define TX_PAGE_MAX_COUNT   (32u)           /* Maximum number of pages on TX buffer */
#define DMA_I2S_FLAG_BIT    (0x01u)         /* Bit flag for DMA I2S events */
#define DMA_PDM_FLAG_BIT    (0x02u)         /* Bit flag for DMA PDM events */
#define RECORD_FLAG_BIT     (0x04u)         /* Bit flag for record */

#endif
/* [] END OF FILE */


