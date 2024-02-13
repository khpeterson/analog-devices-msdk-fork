/******************************************************************************
 *
 * Copyright (C) 2022-2023 Maxim Integrated Products, Inc. All Rights Reserved.
 * (now owned by Analog Devices, Inc.),
 * Copyright (C) 2023 Analog Devices, Inc. All Rights Reserved. This software
 * is proprietary to Analog Devices, Inc. and its licensors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************************/

/**
 * @file    main.c
 * @brief   Parallel camera example with the OV7692/OV5642/OV5640/HM01B0/HM0360/PAG7920 camera sensors as defined in the makefile.
 *
 * @details This example uses the UART to stream out the image captured from the camera.
 *          Alternatively, it can display the captured image on TFT is it is enabled in the make file.
 *          The image is prepended with a header that is interpreted by the grab_image.py
 *          python script.  The data from this example through the UART is in a binary format.
 *          Instructions: 1) Load and execute this example. The example will initialize the camera
 *                        and start the repeating binary output of camera frame data.
 *                        2) Run 'sudo grab_image.py /dev/ttyUSB0 921600'
 *                           Substitute the /dev/ttyUSB0 string for the serial port on your system.
 *                           The python program will read in the binary data from this example and
 *                           output a png image.
 */

/***** Includes *****/
#include <stdio.h>
#include <stdint.h>
#include "mxc_sys.h"
#include "mxc.h"
#include "mxc_device.h"
#include "mxc_delay.h"
#include "uart.h"
#include "led.h"
#include "board.h"
#include "camera.h"
#include "mipi_camera.h"
#include "utils.h"
#include "dma.h"

#ifdef TFT_ADAFRUIT
#include "adafruit_3315_tft.h"
#endif
#ifdef TFT_NEWHAVEN
#include "tft_st7789v.h"
#endif

#define CAMERA_FREQ (10 * 1000 * 1000)

// Display captured image on TFT. Disable if image is sent to serial interface
#define ENABLE_TFT

#if defined(CAMERA_HM01B0)
#define IMAGE_XRES 324 / 2
#define IMAGE_YRES 244 / 2
#define CAMERA_MONO
#endif

#if defined(CAMERA_HM0360_MONO) || defined(CAMERA_PAG7920)
#define IMAGE_XRES 320
#define IMAGE_YRES 240
#define CAMERA_MONO
#endif

#if defined(CAMERA_OV7692) || defined(CAMERA_OV5642) || defined(CAMERA_OV5640)
#define IMAGE_XRES 320
#define IMAGE_YRES 240
#endif

#define CON_BAUD \
    115200 * 8 //UART baudrate used for sending data to PC, use max 921600 for serial stream
#define X_START 0
#define Y_START 0

void process_img(void)
{
    uint8_t *raw;
    uint32_t imgLen;
    uint32_t w, h;

    // Get the details of the image from the camera driver.
    camera_get_image(&raw, &imgLen, &w, &h);

    // buffers the entire image and send to serial or TFT
#ifndef ENABLE_TFT
    // Send the image through the UART to the console if no TFT
    // A python program will read from the console and write
    // to an image file
    utils_send_img_to_pc(raw, imgLen, w, h, camera_get_pixel_format());
#else
#ifdef TFT_NEWHAVEN
    MXC_TFT_Stream(X_START, Y_START, IMAGE_XRES, IMAGE_YRES);
    // Stream captured image to TFT display
    TFT_SPI_Transmit(raw, IMAGE_XRES * IMAGE_YRES * 2);
#else //#ifdef TFT_NEWHAVEN
#ifndef CAMERA_MONO
    // Send the image to TFT
    MXC_TFT_WriteBufferRGB565(X_START, Y_START, raw, w, h);
#else
    MXC_TFT_ShowImageCameraMono(X_START, Y_START, raw, w, h);
#endif // #ifndef CAMERA_MONO
#endif //#ifdef TFT_NEWHAVEN
#endif // #ifndef ENABLE_TFT
}

// *****************************************************************************
int main(void)
{
    int ret = 0;
    int slaveAddress;
    int id;
    int dma_channel;

    /* Enable cache */
    MXC_ICC_Enable(MXC_ICC0);

    /* Set system clock to 100 MHz */
    MXC_SYS_Clock_Select(MXC_SYS_CLOCK_IPO);
    SystemCoreClockUpdate();

    // Initialize DMA for camera interface
    MXC_DMA_Init();
    dma_channel = MXC_DMA_AcquireChannel();

    mxc_uart_regs_t *ConsoleUart = MXC_UART_GET_UART(CONSOLE_UART);

    if ((ret = MXC_UART_Init(ConsoleUart, CON_BAUD, MXC_UART_IBRO_CLK)) != E_NO_ERROR) {
        return ret;
    }

    // Initialize the camera driver.
    camera_init(CAMERA_FREQ);
    printf("\n\nCameraIF Example\n");

    slaveAddress = camera_get_slave_address();
    printf("Camera I2C slave address: %02x\n", slaveAddress);

    // Obtain product ID of the camera.
    ret = camera_get_product_id(&id);

    if (ret != STATUS_OK) {
        printf("Error returned from reading camera id. Error %d\n", ret);
        return -1;
    }

    printf("Camera ID detected: %04x\n", id);

#if defined(CAMERA_HM01B0) || defined(CAMERA_HM0360_MONO) || defined(CAMERA_OV5642) || \
    defined(CAMERA_OV5640)
    camera_set_vflip(0);
#ifdef TFT_NEWHAVEN
    // Newhaven display needs a horizontal flip to match orientation
    camera_set_hmirror(1);
#else
    camera_set_hmirror(0);
#endif

#endif

#if defined(CAMERA_OV7692)
    camera_set_hmirror(0);
#endif

#ifdef ENABLE_TFT
    printf("Init TFT\n");
    /* Initialize TFT display */
#ifdef TFT_ADAFRUIT
    if (MXC_TFT_Init(MXC_SPI0, 1, NULL, NULL) != E_NO_ERROR) {
        printf("TFT initialization failed\n");
        return E_ABORT;
    }
#else
    /* Initialize TFT display */
    if (MXC_TFT_Init(NULL, NULL) != E_NO_ERROR) {
        printf("TFT initialization failed\n");
        return E_ABORT;
    }
#endif
    MXC_TFT_SetRotation(ROTATE_90);
    MXC_TFT_SetBackGroundColor(4);
#endif

    // Setup the camera image dimensions, pixel format and data acquiring details.
#ifndef CAMERA_MONO
#if defined(OV5640_DVP)
    ret = camera_setup(IMAGE_XRES, IMAGE_YRES, PIXEL_FORMAT_RGB565, FIFO_FOUR_BYTE, USE_DMA,
                       dma_channel); // RGB565
#else
    ret = camera_setup(IMAGE_XRES, IMAGE_YRES, PIXFORMAT_RGB565, FIFO_FOUR_BYTE, USE_DMA,
                       dma_channel); // RGB565
#endif
#else
    ret = camera_setup(IMAGE_XRES, IMAGE_YRES, PIXFORMAT_BAYER, FIFO_FOUR_BYTE, USE_DMA,
                       dma_channel); // Mono
#endif

    if (ret != STATUS_OK) {
        printf("Error returned from setting up camera. Error %d\n", ret);
        return -1;
    }

#if defined(OV5640_DVP)
    camera_write_reg(0x503d, 0x0); // workaround: disable test pattern: color bar
#endif

    // Start capturing a first  camera image frame.
    camera_start_capture_image();
    printf("Capture image\n");

    while (1) {
        // Check if image is acquired.
        if (camera_is_image_rcv()) {
            // Process the image, send it through the UART console.
            process_img();

            // Prepare for another frame capture.
            LED_Toggle(LED1);
            camera_start_capture_image();
            printf(".\n");
        }
    }

    return ret;
}
