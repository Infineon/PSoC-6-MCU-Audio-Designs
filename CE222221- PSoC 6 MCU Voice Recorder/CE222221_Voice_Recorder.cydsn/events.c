/******************************************************************************
* File Name: events.c
*
* Version: 1.0
*
* Description: This file contains the functions used to handle events.
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
#include "events.h"
#include "touch.h"
#include "recorder.h"
#include "codec.h"
#include "graphics.h"

#include "rtos.h"

/*******************************************************************************
* Function Name: EventsTask
********************************************************************************
* Summary:
*   This function handle any events triggered by touch and recording.
*
* Parameters:
*   arg: Required argument for task function.
*
*******************************************************************************/
void EventsTask(void *arg)
{
    uint32_t event;
    uint32_t graphics_event;
    recorder_states_t  state;
    int32_t volume = CODEC_HP_DEFAULT_VOLUME;
    
    (void) arg;
        
    /* Update graphics to show STOP icon */
    graphics_event = SHOW_STOP;
    xQueueSend(GUIQueue, &graphics_event, 0);
    
    /* Update graphics to show the volume text */
    graphics_event = SHOW_VOLUME_TXT;
    xQueueSend(GUIQueue, &graphics_event, 0);
    
    /* Update graphics to show the volume value */
    graphics_event = SHOW_VOLUME_VAL | VOLUME_IN_PERCENT(CODEC_HP_DEFAULT_VOLUME);
    xQueueSend(GUIQueue, &graphics_event, 0);
    
    /* Update graphics to show the time */
    graphics_event = SHOW_TIMER;
    xQueueSend(GUIQueue, &graphics_event, 0);
    
    /* Initialize the audio codec */
    Codec_Init();
    
    /* Activate the audio codec */
    Codec_Activate();
        
    /* Set default volume */
    while (Codec_AdjustBothHeadphoneVolume(CODEC_HP_DEFAULT_VOLUME) != CY_SCB_I2C_SUCCESS) {};
    
    while (1)
    {
        /* Wait till an event occurs */
        if (xQueueReceive(EventsQueue, &event, portMAX_DELAY))
        {
            /* Capture the current recorder state */
            state = RecorderState();
            
            /* Handle the specific event */
            switch (event)
            {
                /* Touch Events */
                case LEFT_BUTTON:   
                
                    /* This button is the recording button */
                
                    /* If already recording, stop recording */
                    if (state == RECORDING)
                    {       
                        graphics_event = SHOW_STOP;
                        xQueueSend(GUIQueue, &graphics_event, 0);
                        
                        StopRecorder();
                    }
                    /* If playing or pause, reset the recorder */
                    else if ((state == PLAYING) || (state == PAUSED))
                    {
                        graphics_event = SHOW_STOP;
                        xQueueSend(GUIQueue, &graphics_event, 0);
                        
                        ResetRecorder();
                    }
                    /* In any other state, start recording */
                    else
                    {
                        graphics_event = SHOW_RECORDING;
                        xQueueSend(GUIQueue, &graphics_event, 0);
                        
                        graphics_event = SHOW_NO_WARNING;
                        xQueueSend(GUIQueue, &graphics_event, 0);
                                                  
                        StartRecorder();
                    }                    
                    
                    break;
                case RIGHT_BUTTON:
                    
                    /* This button is the playing button */
                    
                    /* Clear warning message */
                    graphics_event = SHOW_NO_WARNING;
                    xQueueSend(GUIQueue, &graphics_event, 0);
                    
                    /* If recording, stop recording and play the record */
                    if (state == RECORDING)
                    {
                        graphics_event = SHOW_PLAYING;
                        xQueueSend(GUIQueue, &graphics_event, 0);
                        
                        StopRecorder();
                        PlayRecorder();
                    }
                    /* If playing, pause the playing */
                    else if (state == PLAYING)
                    {
                        graphics_event = SHOW_PAUSE;
                        xQueueSend(GUIQueue, &graphics_event, 0);
                        
                        PauseRecorder();
                    } 
                    /* If paused, resume playing */
                    else if (state == PAUSED)
                    {
                        graphics_event = SHOW_PLAYING;
                        xQueueSend(GUIQueue, &graphics_event, 0); 
                        
                        ResumeRecorder();
                    }
                    /* If in idle state, start playing */
                    else if (state == IDLE)
                    {
                        graphics_event = SHOW_PLAYING;
                        xQueueSend(GUIQueue, &graphics_event, 0);
                        
                        PlayRecorder();
                    }                                       
                    
                    break;
                case SLIDER_LEFT:
                    
                    /* This action decrease the speaker volume */
                    
                    volume += EVENT_VOLUME_STEP;
                    
                    if (volume >= CODEC_HP_MUTE_VALUE)
                    {
                        volume = CODEC_HP_MUTE_VALUE;
                    }
                    
                    Codec_AdjustBothHeadphoneVolume(volume);
                    
                    graphics_event = SHOW_VOLUME_VAL | VOLUME_IN_PERCENT(volume);
                    xQueueSend(GUIQueue, &graphics_event, 0);
                    break;
                case SLIDER_RIGHT:    
                    
                    /* This action increase the speaker volume */
                    
                    volume -= EVENT_VOLUME_STEP;
                    
                    if (volume < CODEC_HP_VOLUME_MAX)
                    {
                        volume = CODEC_HP_VOLUME_MAX;
                    }
                    
                    Codec_AdjustBothHeadphoneVolume(volume);
                    
                    graphics_event = SHOW_VOLUME_VAL | VOLUME_IN_PERCENT(volume);
                    xQueueSend(GUIQueue, &graphics_event, 0);
                    break;
                
                /* Recorder Events */
                case PLAY_COMPLETED:
                    graphics_event = SHOW_STOP;
                    xQueueSend(GUIQueue, &graphics_event, 0);
                    break;
                    
                case REACH_MEM_LIMIT:
                    graphics_event = SHOW_STOP;
                    xQueueSend(GUIQueue, &graphics_event, 0);
                    
                    graphics_event = SHOW_WARNING;
                    xQueueSend(GUIQueue, &graphics_event, 0);
                    break;
                    
                default:
                    break;
            }
        }
    }
}

/* [] END OF FILE */
