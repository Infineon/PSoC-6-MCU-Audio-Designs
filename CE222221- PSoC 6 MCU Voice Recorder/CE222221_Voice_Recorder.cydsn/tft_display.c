/******************************************************************************
* File Name: tft_display.c
*
* Version: 1.0
*
* Description: This file contains the functions used to handle the TFT display.
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
#include "tft_display.h"
#include "GraphicLCDIntf.h"

#include "rtos.h"


/*******************************************************************************
* Function Name: TFT_WriteData
********************************************************************************
* Summary:
*   This function writes data to the TFT display interface.
*
* Parameters:
*   data: Data to be sent to the TFT display
*
*******************************************************************************/
void TFT_WriteData(uint8_t data)
{
    GraphicLCDIntf_Write8(1, data);
}

/*******************************************************************************
* Function Name: TFT_WriteCommand
********************************************************************************
* Summary:
*   This function writes a command to the TFT display interface.
*
* Parameters:
*   command: Command to be sent to the TFT display
*
*******************************************************************************/
void TFT_WriteCommand(uint8_t command)
{
    GraphicLCDIntf_Write8(0, command);
}

/*******************************************************************************
* Function Name: TFT_SetPixel
********************************************************************************
* Summary:
*   This function set a color to a given pixel.
*
* Parameters:
*   x: Column value of the pixel
*   y: Row value of the pixel
*   color: Color of the pixel
*
*******************************************************************************/
void TFT_SetPixel(int16_t x, int16_t y, uint32_t color)
{
    /*First set the Column Start and End, set to same since this writes one pixel*/
    TFT_WriteCommand(0x2A);
    TFT_WriteData((uint8_t)(x>>8));
    TFT_WriteData((uint8_t)(x));
    TFT_WriteData((uint8_t)(x>>8));
    TFT_WriteData((uint8_t)(x));

    /*First set the Row Start and End, set to same since this writes one pixel*/
    TFT_WriteCommand(0x2B);
    TFT_WriteData((uint8_t)(y>>8));
    TFT_WriteData((uint8_t)(y));
    TFT_WriteData((uint8_t)(y>>8));
    TFT_WriteData((uint8_t)(y));

    /*Write the RGB data for specified pixel*/
    TFT_WriteCommand(0x2C);
    TFT_WriteData((uint8_t) (color>>16) );  //RED
    TFT_WriteData((uint8_t) (color>>8) );   //GREEN
    TFT_WriteData((uint8_t) (color));       //BLUE
    
    vTaskDelay(1);
}

/*******************************************************************************
* Function Name: TFT_Init
********************************************************************************
* Summary: 
*   This function updates the display based on events that occur.
*
*******************************************************************************/
void TFT_Init(void)
{
    /* Start the GraphicLCD component */
    GraphicLCDIntf_Start();
    
    vTaskDelay(pdMS_TO_TICKS(20));
    Cy_GPIO_Clr(Intf_nreset_0_PORT, Intf_nreset_0_NUM);
    vTaskDelay(pdMS_TO_TICKS(75));
    Cy_GPIO_Set(Intf_nreset_0_PORT, Intf_nreset_0_NUM);
    vTaskDelay(pdMS_TO_TICKS(200));
    
    TFT_WriteCommand(0x28);
    TFT_WriteCommand(0x11); //Exit Sleep mode
    vTaskDelay(pdMS_TO_TICKS(100));
    
    TFT_WriteCommand(0x36);
    TFT_WriteData(0xA0);//MADCTL: memory data access control
    TFT_WriteCommand(0x3A);
    TFT_WriteData(0x66);//COLMOD: Interface Pixel format
    TFT_WriteCommand(0xB2);
    TFT_WriteData(0x0C);
    TFT_WriteData(0x0C);
    TFT_WriteData(0x00);
    TFT_WriteData(0x33);
    TFT_WriteData(0x33);//PORCTRK: Porch setting
    TFT_WriteCommand(0xB7);
    TFT_WriteData(0x35);//GCTRL: Gate Control
    TFT_WriteCommand(0xBB);
    TFT_WriteData(0x2B);//VCOMS: VCOM setting
    TFT_WriteCommand(0xC0);
    TFT_WriteData(0x2C);//LCMCTRL: LCM Control
    TFT_WriteCommand(0xC2);
    TFT_WriteData(0x01);
    TFT_WriteData(0xFF);//VDVVRHEN: VDV and VRH Command Enable
    TFT_WriteCommand(0xC3);
    TFT_WriteData(0x11);//VRHS: VRH Set
    TFT_WriteCommand(0xC4);    
    TFT_WriteData(0x20);//VDVS: VDV Set
    TFT_WriteCommand(0xC6);
    TFT_WriteData(0x0F);//FRCTRL2: Frame Rate control in normal mode
    TFT_WriteCommand(0xD0);
    TFT_WriteData(0xA4);
    TFT_WriteData(0xA1);//PWCTRL1: Power Control 1
    TFT_WriteCommand(0xE0);
    TFT_WriteData(0xD0);
    TFT_WriteData(0x00);
    TFT_WriteData(0x05);
    TFT_WriteData(0x0E);
    TFT_WriteData(0x15);
    TFT_WriteData(0x0D);
    TFT_WriteData(0x37);
    TFT_WriteData(0x43);
    TFT_WriteData(0x47);
    TFT_WriteData(0x09);
    TFT_WriteData(0x15);
    TFT_WriteData(0x12);
    TFT_WriteData(0x16);
    TFT_WriteData(0x19);//PVGAMCTRL: Positive Voltage Gamma control
    TFT_WriteCommand(0xE1);
    TFT_WriteData(0xD0);
    TFT_WriteData(0x00);
    TFT_WriteData(0x05);
    TFT_WriteData(0x0D);
    TFT_WriteData(0x0C);
    TFT_WriteData(0x06);
    TFT_WriteData(0x2D);
    TFT_WriteData(0x44);
    TFT_WriteData(0x40);
    TFT_WriteData(0x0E);
    TFT_WriteData(0x1C);
    TFT_WriteData(0x18);
    TFT_WriteData(0x16);
    TFT_WriteData(0x19);//NVGAMCTRL: Negative Voltage Gamma control
    TFT_WriteCommand(0x2B);
    TFT_WriteData(0x00);
    TFT_WriteData(0x00);
    TFT_WriteData(0x00);
    TFT_WriteData(0xEF);//Y address set
    TFT_WriteCommand(0x2A);
    TFT_WriteData(0x00);
    TFT_WriteData(0x00);
    TFT_WriteData(0x01);
    TFT_WriteData(0x3F);//X address set
    vTaskDelay(pdMS_TO_TICKS(10));
    TFT_WriteCommand(0x29);
    
    uint16 row, column;
    TFT_WriteCommand(0x2C);
    for(row = 0; row < TFT_WIDTH; row++)
    {
        for(column = 0; column < TFT_HEIGHT; column++)
        {
            {
                TFT_WriteData(0x00); //Red
                TFT_WriteData(0x00); //Green
                TFT_WriteData(0x00); //Blue
            }
        }
    }
}

/* [] END OF FILE */
