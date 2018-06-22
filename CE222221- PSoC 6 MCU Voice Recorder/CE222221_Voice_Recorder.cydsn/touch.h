/******************************************************************************
* File Name: touch.h
*
* Version: 1.0
*
* Description: This file declares the functions provided by the touch.c file
*
* Related Document: N/A
*
* Hardware Dependency: * Hardware Dependency: CY8CKIT-062-WiFi-BT PSoC 6 WiFi-BT Pioneer Kit
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
/*****************************************************************************
* This file declares the functions provided by the touch.c file
*******************************************************************************/

/* Include Guard */
#ifndef TOUCH_H
#define TOUCH_H

    /* Header file includes */    
    #include <project.h>    

    /* Total number of touch types */
    #define NUMBER_OF_TOUCH_TYPES (uint8_t)(0x05u)
               
    /* Enumerated data type for different types of CapSense touch. Integer values 
       are added so that this data type can be used to as a parameter to function 
       pointers */
    typedef enum
    {
        NO_TOUCH        = 0x00000000u,
        LEFT_BUTTON     = 0x10000001u,
        RIGHT_BUTTON    = 0x10000002u,
        SLIDER_LEFT     = 0x10000003u,
        SLIDER_RIGHT    = 0x10000004u
    }   touch_data_types_t;

    /* Data type that stores touch information */
    typedef struct
    {
        touch_data_types_t  touchType;
        bool                scanBusy;
    }   touch_data_t;

    /* Function that initializes the CapSense touch sensing */
    void       InitTouch(void);

    /* Function to read and analyze the touch information from the CapSense sensors */
    void TouchTask(void *arg);

#endif
/* [] END OF FILE */
