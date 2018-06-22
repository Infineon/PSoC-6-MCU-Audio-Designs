/******************************************************************************
* File Name: graphics.c
*
* Version: 1.0
*
* Description: This file contains the functions used to handle the graphic
* user interface.
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
#include "graphics.h"
#include "tft_display.h"
#include "ugui.h"
#include <stdio.h>
#include "project.h"

#include "rtos.h"

/* Global Variables */
UG_GUI gui;

/* Local Functions */

/* Draw the Recording icon (Full red circle) */
static void GraphicsDrawRecordingIcon(void)
{
    UG_FillFrame(TFT_WIDTH/2-GUI_ICON_SIZE, TFT_HEIGHT/2-GUI_ICON_SIZE,
                 TFT_WIDTH/2+GUI_ICON_SIZE, TFT_HEIGHT/2+GUI_ICON_SIZE, C_BLACK);
    UG_FillCircle(TFT_WIDTH/2 , TFT_HEIGHT/2, GUI_ICON_SIZE, C_RED);
}

/* Draw the Playing icon (Full green triangle) */
static void GraphicsDrawPlayingIcon(void)
{
    uint32_t index;
    
    for (index = 0; index <= GUI_ICON_SIZE*2; index++)
    {   
        UG_DrawLine(TFT_WIDTH/2-GUI_ICON_SIZE + index, TFT_HEIGHT/2-GUI_ICON_SIZE,
                    TFT_WIDTH/2-GUI_ICON_SIZE + index, TFT_HEIGHT/2+GUI_ICON_SIZE+index/2, C_BLACK);
        
        UG_DrawLine(TFT_WIDTH/2-GUI_ICON_SIZE + index, TFT_HEIGHT/2-GUI_ICON_SIZE+index/2,
                    TFT_WIDTH/2-GUI_ICON_SIZE + index, TFT_HEIGHT/2+GUI_ICON_SIZE-index/2, C_GREEN);
        
        UG_DrawLine(TFT_WIDTH/2-GUI_ICON_SIZE + index, TFT_HEIGHT/2+GUI_ICON_SIZE,
                    TFT_WIDTH/2-GUI_ICON_SIZE + index, TFT_HEIGHT/2+GUI_ICON_SIZE-index/2, C_BLACK);
    }
}

/* Draw the Stop icon (Full blue square) */
static void GraphicsDrawStoppedIcon(void)
{
    UG_FillFrame(TFT_WIDTH/2-GUI_ICON_SIZE, TFT_HEIGHT/2-GUI_ICON_SIZE,
                 TFT_WIDTH/2+GUI_ICON_SIZE, TFT_HEIGHT/2+GUI_ICON_SIZE, C_BLUE);
}

/* Draw the Paused icon (Full two yellow square) */
static void GraphicsDrawPausedIcon(void)
{
    UG_FillFrame(TFT_WIDTH/2-GUI_ICON_SIZE, TFT_HEIGHT/2-GUI_ICON_SIZE,
                 TFT_WIDTH/2-GUI_ICON_SIZE/3, TFT_HEIGHT/2+GUI_ICON_SIZE, C_YELLOW);    
    
    UG_FillFrame(TFT_WIDTH/2-GUI_ICON_SIZE/3, TFT_HEIGHT/2-GUI_ICON_SIZE,
                 TFT_WIDTH/2+GUI_ICON_SIZE/3, TFT_HEIGHT/2+GUI_ICON_SIZE, C_BLACK);
    
    UG_FillFrame(TFT_WIDTH/2+GUI_ICON_SIZE/3, TFT_HEIGHT/2-GUI_ICON_SIZE,
                 TFT_WIDTH/2+GUI_ICON_SIZE, TFT_HEIGHT/2+GUI_ICON_SIZE, C_YELLOW);
    
}

/* Draw the volume text on the bottom of the screen */
static void GraphicsDrawVolumeText(void)
{
    UG_SetForecolor(C_WHITE);
    
    UG_PutString(0,TFT_HEIGHT-TEXT_SIZE.char_height, " Vol:    %");
}

/* Update the volume value */
static void GraphicsUpdateVolume(uint32_t volume)
{
    char string[TEXT_BUFFER_SIZE]; 
    
    UG_SetForecolor(C_WHITE);
    
    sprintf(string, "%3u", (uint8_t) volume);
    UG_PutString(TEXT_SIZE.char_width*TEXT_VOL_OFFSET,
                 TFT_HEIGHT-TEXT_SIZE.char_height, string);
}

/* Draw warning message that memory reach limit */
static void GraphicsDrawWarningMessage(bool show)
{
    UG_SetForecolor(C_YELLOW);
    
    if (show)
    {
        UG_PutString(0,0,"Warning: Limit Reached!");
    }
    else
    {
        UG_PutString(0,0,"                       ");
    }
}

/* Draw the current recording/playing time */
static void GraphicsUpdateTime(uint32_t time)
{
    char string[TEXT_BUFFER_SIZE];
    
    UG_SetForecolor(C_WHITE);
    
    sprintf(string, "%2u:%02u", (uint8_t) (time / 60), (uint8_t) (time % 60));
    
    UG_PutString(TEXT_SIZE.char_width*TEXT_TIME_OFFSET,
                 TFT_HEIGHT-TEXT_SIZE.char_height, string);
}

/*******************************************************************************
* Function Name: GraphicsTask
********************************************************************************
* Summary:
*   This function updates the display based on events that occur.
*
* Parameters:
*   arg: Required argument for task function.
*
*******************************************************************************/
void GraphicsTask(void *arg)
{
    uint32_t event;
    
    /* Initialize the TFT driver */
    TFT_Init();
       
    /* Start the uGUI graphical library */
    UG_Init(&gui, TFT_SetPixel, TFT_WIDTH, TFT_HEIGHT);
      
    /* Set background color and font size */
    UG_SetBackcolor(C_BLACK);
    UG_FontSelect(&FONT_10X16);
    
    (void) arg;
    
    while (1)
    {
        /* Wait till an action to update the display is received */
        if (xQueueReceive(GUIQueue, &event, portMAX_DELAY))
        {
            switch (event)
            {
                case SHOW_RECORDING:
                    GraphicsDrawRecordingIcon();
                    break;
                case SHOW_PLAYING:
                    GraphicsDrawPlayingIcon();
                    break;
                case SHOW_PAUSE:
                    GraphicsDrawPausedIcon();
                    break;
                case SHOW_STOP:
                    GraphicsDrawStoppedIcon();
                    break;
                case SHOW_VOLUME_TXT:
                    GraphicsDrawVolumeText();
                    break;
                case SHOW_WARNING:
                    GraphicsDrawWarningMessage(true);
                    break;
                case SHOW_NO_WARNING:
                    GraphicsDrawWarningMessage(false);
                    break;
                default: 
                    /* Update Volume on display */
                    if ((event & GUI_EVENT_MASK) == SHOW_VOLUME_VAL)
                    {
                        GraphicsUpdateVolume(CY_LO8(event));
                    }
                    /* Update time on display */
                    else if ((event & GUI_EVENT_MASK) == SHOW_TIMER)
                    {
                        GraphicsUpdateTime(CY_LO16(event));
                    }
                    break;
            }
        }
    }
}

/* [] END OF FILE */
