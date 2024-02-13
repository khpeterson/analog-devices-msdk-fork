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

// facedet_tinierssd
// This file was @generated by ai8xize.py --test-dir sdk/Examples/MAX78000/CNN --prefix facedet_tinierssd --checkpoint-file trained/ai85-facedet-tinierssd-qat8-q.pth.tar --config-file networks/ai85-facedet-tinierssd.yaml --sample-input tests/sample_vggface2_facedetection.npy --fifo --device MAX78000 --timer 0 --display-checkpoint --verbose

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "mxc.h"
#include "cnn.h"
#include "mxc_errors.h"
#include "camera.h"
#include "post_process.h"

#define CAMERA_FREQ 10000000

volatile uint32_t cnn_time; // Stopwatch
extern volatile uint8_t face_detected;

void fail(void)
{
    printf("\n*** FAIL ***\n\n");
    while (1)
        ;
}

// Data input: HWC 3x224x168 (112896 bytes total / 37632 bytes per channel):
void load_input(void)
{
    uint8_t *raw;
    uint32_t imglen, w, h;
    camera_sleep(0); // Wake up camera
    camera_start_capture_image();
    while (!camera_is_image_rcv()) {}
    camera_sleep(1); // Sleep camera to preserve power

    camera_get_image(&raw, &imglen, &w, &h);
    color_correct(raw, w, h);

    // The model needs 168x224.
    // The HM0360 give us 320x240.
    // We will achieve this by debayering "on the fly" to crop to
    // 224x168 while feeding the CNN column-wise instead of row-wise.
    unsigned int crop_x = 224, crop_y = 168;
    unsigned int start_x = (320 - crop_x) >> 1;
    unsigned int start_y = (240 - crop_y) >> 1;

    uint8_t ur = 0, ug = 0, ub = 0;
    int8_t r = 0, g = 0, b = 0;
    uint32_t rgb888 = 0;

    uint16_t rgb565_buffer[crop_y];
    uint16_t rgb565;

    for (unsigned int x = start_x; x < start_x + crop_x; x++) {
        bayer_bilinear_demosaicing_crop(raw, w, h, x, start_y, rgb565_buffer, 1, crop_y);
        /* ^ The crop is achieved here by offsetting where we start
        iterating across the source image.  The rotation is achieved
        by iterating across the image in the vertical (h) direction
        down a single column.  Then, we feed the column into the CNN...
        */

        for (unsigned int y = 0; y < crop_y; y++) { // ... here.
            // Since the CNN expects
            // Decode RGB565 to RGB888
            rgb565 = rgb565_buffer[y];
            ur = (rgb565 & 0xF8);
            ug = (rgb565 & 0b111) << 5;
            ug |= (rgb565 & 0xe000) >> 11;
            ub = (rgb565 & 0x1f00) >> 5;

            // Normalize from [0, 255] -> [-128, 127]
            r = ur - 128;
            g = ug - 128;
            b = ub - 128;

            // Pack to RGB888 (0x00BBGGRR)
            rgb888 = r | (g << 8) | (b << 16);

            // Loading data into the CNN fifo
            while (((*((volatile uint32_t *)0x50000004) & 1)) != 0)
                ; // Wait for FIFO 0
            *((volatile uint32_t *)0x50000008) = rgb888; // Write FIFO 0
        }
    }
}

int main(void)
{
    int ret = 0, id = 0;
    MXC_ICC_Enable(MXC_ICC0); // Enable cache

    // Switch to 100 MHz clock
    MXC_SYS_Clock_Select(MXC_SYS_CLOCK_IPO);
    SystemCoreClockUpdate();

    printf("Waiting...\n");

    // DO NOT DELETE THIS LINE:
    MXC_Delay(SEC(2)); // Let debugger interrupt if needed

    // Initialize DMA and acquire a channel for the camera interface to use
    printf("Initializing DMA\n");
    MXC_DMA_Init();
    int dma_channel = MXC_DMA_AcquireChannel();

    // Initialize the camera driver.
    printf("Initializing camera\n");
    camera_init(CAMERA_FREQ);

    int slaveAddress = camera_get_slave_address();
    printf("Camera I2C slave address: %02x\n", slaveAddress);

    ret = camera_get_manufacture_id(&id);
    if (ret != STATUS_OK) {
        printf("Error returned from reading camera id. Error %d\n", ret);
        return -1;
    }
    printf("Camera ID detected: %04x\n", id);

    camera_set_hmirror(0);
    camera_set_vflip(0);

    ret = camera_setup(320, // width
                       240, // height
                       PIXFORMAT_BAYER, // pixel format
                       FIFO_FOUR_BYTE, // FIFO mode (four bytes is suitable for most cases)
                       USE_DMA, // DMA (enabling DMA will drastically decrease capture time)
                       dma_channel); // Allocate the DMA channel retrieved in initialization

    if (ret != E_NO_ERROR) {
        printf("Failed to setup camera!\n");
        return ret;
    }

    printf("\n*** CNN Inference Test facedet_tinierssd ***\n");

    while (1) {
        face_detected = 0;
        LED_On(1);

        // Switch to high power fast IPO clock
        MXC_SYS_ClockEnable(MXC_SYS_CLOCK_IPO);
        MXC_SYS_Clock_Select(MXC_SYS_CLOCK_IPO);

        // Enable peripheral, enable CNN interrupt, turn on CNN clock
        // CNN clock: APB (50 MHz) div 1
        cnn_enable(MXC_S_GCR_PCLKDIV_CNNCLKSEL_PCLK, MXC_S_GCR_PCLKDIV_CNNCLKDIV_DIV1);
        cnn_init(); // Bring state machine into consistent state
        cnn_load_weights(); // Load kernels
        cnn_load_bias();
        cnn_configure(); // Configure state machine
        cnn_start(); // Start CNN processing
        load_input(); // Load data input via FIFO

        while (cnn_time == 0) MXC_LP_EnterSleepMode(); // Wait for CNN

        // Run Non-Maximal Suppression (NMS) on bounding boxes
        get_priors();
        localize_objects();
        LED_Off(1);

        if (!face_detected) {
            LED_Off(0);
        } else {
            LED_On(0);
        }

        cnn_disable(); // Shut down CNN clock, disable peripheral

        // Switch to low power IBRO clock
        MXC_SYS_ClockEnable(MXC_SYS_CLOCK_IBRO);
        MXC_SYS_Clock_Select(MXC_SYS_CLOCK_IBRO);
        MXC_SYS_ClockDisable(MXC_SYS_CLOCK_IPO);

        MXC_Delay(MXC_DELAY_MSEC(150)); // Slight delay to allow LED to be seen

#ifdef CNN_INFERENCE_TIMER
        printf("Approximate data loading and inference time: %u us\n\n", cnn_time);
#endif
    }

    return 0;
}

/*
  SUMMARY OF OPS
  Hardware: 589,595,888 ops (588,006,720 macc; 1,589,168 comp; 0 add; 0 mul; 0 bitwise)
    Layer 0: 4,327,680 ops (4,064,256 macc; 263,424 comp; 0 add; 0 mul; 0 bitwise)
    Layer 1: 11,063,808 ops (10,838,016 macc; 225,792 comp; 0 add; 0 mul; 0 bitwise)
    Layer 2: 43,502,592 ops (43,352,064 macc; 150,528 comp; 0 add; 0 mul; 0 bitwise)
    Layer 3: 86,854,656 ops (86,704,128 macc; 150,528 comp; 0 add; 0 mul; 0 bitwise)
    Layer 4: 86,854,656 ops (86,704,128 macc; 150,528 comp; 0 add; 0 mul; 0 bitwise)
    Layer 5: 86,854,656 ops (86,704,128 macc; 150,528 comp; 0 add; 0 mul; 0 bitwise)
    Layer 6: 173,709,312 ops (173,408,256 macc; 301,056 comp; 0 add; 0 mul; 0 bitwise)
    Layer 7 (backbone_conv8): 86,779,392 ops (86,704,128 macc; 75,264 comp; 0 add; 0 mul; 0 bitwise)
    Layer 8 (backbone_conv9): 5,513,088 ops (5,419,008 macc; 94,080 comp; 0 add; 0 mul; 0 bitwise)
    Layer 9 (backbone_conv10): 1,312,640 ops (1,290,240 macc; 22,400 comp; 0 add; 0 mul; 0 bitwise)
    Layer 10 (conv12_1): 647,360 ops (645,120 macc; 2,240 comp; 0 add; 0 mul; 0 bitwise)
    Layer 11 (conv12_2): 83,440 ops (80,640 macc; 2,800 comp; 0 add; 0 mul; 0 bitwise)
    Layer 12: 1,354,752 ops (1,354,752 macc; 0 comp; 0 add; 0 mul; 0 bitwise)
    Layer 13: 40,320 ops (40,320 macc; 0 comp; 0 add; 0 mul; 0 bitwise)
    Layer 14: 677,376 ops (677,376 macc; 0 comp; 0 add; 0 mul; 0 bitwise)
    Layer 15: 20,160 ops (20,160 macc; 0 comp; 0 add; 0 mul; 0 bitwise)

  RESOURCE USAGE
  Weight memory: 275,184 bytes out of 442,368 bytes total (62.2%)
  Bias memory:   536 bytes out of 2,048 bytes total (26.2%)
*/
