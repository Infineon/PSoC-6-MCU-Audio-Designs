/******************************************************************************
* File Name: display.h
*
* Version: 1.0
*
* Description: This file declares the functions provided by the graphics.c file
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
#ifndef GRAPHICS_H
#define GRAPHICS_H
    
    #include <stdint.h> 
    
    /* Enumerated data type for different types of gui events. Integer values 
       are added so that this data type can be used to as a parameter to function 
       pointers */
    typedef enum
    {
        NO_GUI_EVENT    = 0x00000000u,
        SHOW_RECORDING  = 0x30000001u,
        SHOW_PLAYING    = 0x30000002u,
        SHOW_PAUSE      = 0x30000003u,
        SHOW_STOP       = 0x30000004u,
        SHOW_WARNING    = 0x30000005u,
        SHOW_NO_WARNING = 0x30000006u,
        SHOW_VOLUME_TXT = 0x30000007u,
        SHOW_VOLUME_VAL = 0x30010000u,
        SHOW_TIMER      = 0x30020000u,
    }   gui_events_t;
    
    #define GUI_ICON_SIZE           25u         /* Size of the icons */
    #define TEXT_SIZE               FONT_10X16  /* Size of the text */
    #define TEXT_VOL_OFFSET         6u          /* Text volume offset */
    #define TEXT_TIME_OFFSET        24u         /* Text time offset */
    #define TEXT_BUFFER_SIZE        8u          /* Text buffer size for volume */
    #define GUI_EVENT_MASK          0xFFFF0000u /* GUI Event Mask */
    
    void GraphicsTask(void *arg);
    
#endif

/* [] END OF FILE */
