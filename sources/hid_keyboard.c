/*
 * The Clear BSD License
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted (subject to the limitations in the disclaimer below) provided
 * that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY THIS LICENSE.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "usb_device_config.h"
#include "usb.h"
#include "usb_device.h"
#include "usb_device_class.h"
#include "usb_device_hid.h"
#include "usb_device_ch9.h"
#include "usb_device_descriptor.h"
#include "composite.h"
#include "hid_keyboard.h"
#include "hid_mouse.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

static usb_status_t USB_DeviceHidKeyboardAction(void);

/*******************************************************************************
 * Variables
 ******************************************************************************/

USB_DMA_NONINIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE) static uint8_t s_KeyboardBuffer[USB_HID_KEYBOARD_REPORT_LENGTH];
static usb_device_composite_struct_t *s_UsbDeviceComposite;
static usb_device_hid_keyboard_struct_t s_UsbDeviceHidKeyboard;

/*******************************************************************************
 * Code
 ******************************************************************************/
extern uint8_t flag_notepad;

static usb_status_t USB_DeviceHidKeyboardAction(void)
{
    static int wait = 0U;
    enum
    {
        OPEN_EJECUTOR,
        WRITE_PROGRAM,
		WRITE_ENTER
    };

    static uint8_t state = OPEN_EJECUTOR;

    s_UsbDeviceHidKeyboard.buffer[2] = 0x00U;
    s_UsbDeviceHidKeyboard.buffer[3] = 0x00U;
    s_UsbDeviceHidKeyboard.buffer[4] = 0x00U;
    s_UsbDeviceHidKeyboard.buffer[5] = 0x00U;
    s_UsbDeviceHidKeyboard.buffer[6] = 0x00U;
    s_UsbDeviceHidKeyboard.buffer[7] = 0x00U;

    switch (state)
    {
        case OPEN_EJECUTOR:
            wait++;
            if (200U == wait)
            {
                s_UsbDeviceHidKeyboard.buffer[2] = KEY_RIGHT_GUI;
                s_UsbDeviceHidKeyboard.buffer[3] = KEY_R;
                state++;
                wait = 0;
            }
            break;
        case WRITE_PROGRAM:
            wait++;
            if (200U == wait)
            {
                s_UsbDeviceHidKeyboard.buffer[2] = KEY_M;
                s_UsbDeviceHidKeyboard.buffer[3] = KEY_S;
                s_UsbDeviceHidKeyboard.buffer[4] = KEY_P;
                s_UsbDeviceHidKeyboard.buffer[5] = KEY_A;
                s_UsbDeviceHidKeyboard.buffer[6] = KEY_I;
                s_UsbDeviceHidKeyboard.buffer[7] = KEY_N;
                state++;
                wait = 0;
            }
            break;
        case WRITE_ENTER:
            wait++;
            if (200U == wait)
            {
                s_UsbDeviceHidKeyboard.buffer[2] = KEY_T;
                s_UsbDeviceHidKeyboard.buffer[3] = KEY_ENTER;
                state++;
                wait = 0;
            }
            break;
        default:
            wait++;
            if (200U == wait)
            {
            	state++;
            	wait = 0;
            }
            break;
    }
    if(1 == flag_notepad)
    {
    	uint8_t state_two = OPEN_EJECUTOR;
    	switch(state_two)
    	{
    		case OPEN_EJECUTOR:
    			wait++;
    			if(200 == wait)
    			{
                    s_UsbDeviceHidKeyboard.buffer[2] = KEY_RIGHT_GUI;
                    s_UsbDeviceHidKeyboard.buffer[3] = KEY_R;
                    state_two++;
                    wait = 0;
    			}
    		break;
    		case WRITE_PROGRAM:
    			wait++;
    			if (wait == 200)
    			{
                    s_UsbDeviceHidKeyboard.buffer[2] = KEY_N;
                    s_UsbDeviceHidKeyboard.buffer[3] = KEY_O;
                    s_UsbDeviceHidKeyboard.buffer[4] = KEY_T;
                    s_UsbDeviceHidKeyboard.buffer[5] = KEY_E;
                    s_UsbDeviceHidKeyboard.buffer[6] = KEY_P;
                    s_UsbDeviceHidKeyboard.buffer[7] = KEY_A;
                    state_two++;
                    wait = 0;
    			}
    		case WRITE_ENTER:
    			wait++;
    			if (wait == 200)
    			{
                    s_UsbDeviceHidKeyboard.buffer[2] = KEY_T;
                    s_UsbDeviceHidKeyboard.buffer[3] = KEY_ENTER;
                    state++;
                    wait = 0;
    			}

    		break;
    	}
    }
    return USB_DeviceHidSend(s_UsbDeviceComposite->hidKeyboardHandle, USB_HID_KEYBOARD_ENDPOINT_IN,
                             s_UsbDeviceHidKeyboard.buffer, USB_HID_KEYBOARD_REPORT_LENGTH);
}

usb_status_t USB_DeviceHidKeyboardCallback(class_handle_t handle, uint32_t event, void *param)
{
    usb_status_t error = kStatus_USB_Error;

    switch (event)
    {
        case kUSB_DeviceHidEventSendResponse:
            if (s_UsbDeviceComposite->attach)
            {
                return USB_DeviceHidKeyboardAction();
            }
            break;
        case kUSB_DeviceHidEventGetReport:
        case kUSB_DeviceHidEventSetReport:
        case kUSB_DeviceHidEventRequestReportBuffer:
            error = kStatus_USB_InvalidRequest;
            break;
        case kUSB_DeviceHidEventGetIdle:
        case kUSB_DeviceHidEventGetProtocol:
        case kUSB_DeviceHidEventSetIdle:
        case kUSB_DeviceHidEventSetProtocol:
            break;
        default:
            break;
    }

    return error;
}

usb_status_t USB_DeviceHidKeyboardSetConfigure(class_handle_t handle, uint8_t configure)
{
    if (USB_COMPOSITE_CONFIGURE_INDEX == configure)
    {
        return USB_DeviceHidKeyboardAction(); /* run the cursor movement code */
    }
    return kStatus_USB_Error;
}

usb_status_t USB_DeviceHidKeyboardSetInterface(class_handle_t handle, uint8_t interface, uint8_t alternateSetting)
{
    if (USB_HID_KEYBOARD_INTERFACE_INDEX == interface)
    {
        return USB_DeviceHidKeyboardAction(); /* run the cursor movement code */
    }
    return kStatus_USB_Error;
}

usb_status_t USB_DeviceHidKeyboardInit(usb_device_composite_struct_t *deviceComposite)
{
    s_UsbDeviceComposite = deviceComposite;
    s_UsbDeviceHidKeyboard.buffer = s_KeyboardBuffer;
    return kStatus_USB_Success;
}
