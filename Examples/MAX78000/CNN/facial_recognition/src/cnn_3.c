/******************************************************************************
 *
 * Copyright (C) 2019-2023 Maxim Integrated Products, Inc. All Rights Reserved.
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

// 85_dotprod_112_noffset_new_k
// This file was @generated by ai8xize.py --test-dir sdk/Examples/MAX78000/CNN --prefix 85_dotprod_112_noffset_new_k --checkpoint-file trained/85_dot_emb_112_checkpoint.pth-q.pth.tar --config-file networks/85_dotprod.yaml --overwrite --device MAX78000 --timer 0 --display-checkpoint --verbose

// DO NOT EDIT - regenerate this file instead!

// Configuring 1 layer
// Input data: HWC
// Layer 0: 64x1x1, no pooling, linear, no activation, 1024x1x1 output

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "mxc.h"
#include "gcfr_regs.h"
#include "cnn_1.h"
#include "cnn_3.h"
#include "weights_3.h"

void CNN_3_ISR(void)
{
    // Acknowledge interrupt to all quadrants
    *((volatile uint32_t *)0x50100000) &= ~((1 << 12) | 1);
    *((volatile uint32_t *)0x50500000) &= ~((1 << 12) | 1);
    *((volatile uint32_t *)0x50900000) &= ~((1 << 12) | 1);
    *((volatile uint32_t *)0x50d00000) &= ~((1 << 12) | 1);

    CNN_COMPLETE; // Signal that processing is complete
#ifdef CNN_INFERENCE_TIMER
    cnn_time = MXC_TMR_SW_Stop(CNN_INFERENCE_TIMER);
#else
    cnn_time = 1;
#endif
}

static const uint32_t kernels_3[] = KERNELS_3;

int cnn_3_load_weights(void)
{
    uint32_t len;
    volatile uint32_t *addr;
    const uint32_t *ptr = kernels_3;

    while ((addr = (volatile uint32_t *)*ptr++) != 0) {
        *((volatile uint8_t *)((uint32_t)addr | 1)) = 0x01; // Set address
        len = *ptr++;
        while (len-- > 0) *addr++ = *ptr++;
    }

    return CNN_OK;
}

int cnn_3_load_bias(void)
{
    // Not used in this network
    return CNN_OK;
}

int cnn_3_init(void)
{
    *((volatile uint32_t *)0x50001000) = 0x00000000; // AON control
    *((volatile uint32_t *)0x50100000) = 0x00100008; // Stop SM
    *((volatile uint32_t *)0x50100004) = 0x0000040e; // SRAM control
    *((volatile uint32_t *)0x50500000) = 0x00100008; // Stop SM
    *((volatile uint32_t *)0x50500004) = 0x0000040e; // SRAM control
    *((volatile uint32_t *)0x50900000) = 0x00100008; // Stop SM
    *((volatile uint32_t *)0x50900004) = 0x0000040e; // SRAM control
    *((volatile uint32_t *)0x50d00000) = 0x00100008; // Stop SM
    *((volatile uint32_t *)0x50d00004) = 0x0000040e; // SRAM control

    return CNN_OK;
}

int cnn_3_configure(void)
{
    // Layer 0 quadrant 0
    *((volatile uint32_t *)0x50100310) = 0x00001000; // SRAM write ptr
    *((volatile uint32_t *)0x50100390) = 0x00000001; // Write ptr time slot offs
    *((volatile uint32_t *)0x50100410) = 0x00002000; // Write ptr mask offs
    *((volatile uint32_t *)0x50100490) = 0x00000001; // Write ptr multi-pass channel offs
    *((volatile uint32_t *)0x50100590) = 0x0000e920; // Layer control
    *((volatile uint32_t *)0x50100a10) = 0x0001f8f0; // Layer control 2
    *((volatile uint32_t *)0x50100610) = 0x00001ff8; // Mask offset and count
    *((volatile uint32_t *)0x50100110) = 0x00000100; // 1D
    *((volatile uint32_t *)0x50100790) = 0x00022000; // Post processing register
    *((volatile uint32_t *)0x50100710) = 0xffffffff; // Mask and processor enables

    // Layer 0 quadrant 1
    *((volatile uint32_t *)0x50500310) = 0x00001000; // SRAM write ptr
    *((volatile uint32_t *)0x50500390) = 0x00000001; // Write ptr time slot offs
    *((volatile uint32_t *)0x50500410) = 0x00002000; // Write ptr mask offs
    *((volatile uint32_t *)0x50500490) = 0x00000001; // Write ptr multi-pass channel offs
    *((volatile uint32_t *)0x50500590) = 0x00000920; // Layer control
    *((volatile uint32_t *)0x50500a10) = 0x0001f8f0; // Layer control 2
    *((volatile uint32_t *)0x50500610) = 0x00001ff8; // Mask offset and count
    *((volatile uint32_t *)0x50500110) = 0x00000100; // 1D
    *((volatile uint32_t *)0x50500790) = 0x00022000; // Post processing register
    *((volatile uint32_t *)0x50500710) = 0xffffffff; // Mask and processor enables

    // Layer 0 quadrant 2
    *((volatile uint32_t *)0x50900310) = 0x00001000; // SRAM write ptr
    *((volatile uint32_t *)0x50900390) = 0x00000001; // Write ptr time slot offs
    *((volatile uint32_t *)0x50900410) = 0x00002000; // Write ptr mask offs
    *((volatile uint32_t *)0x50900490) = 0x00000001; // Write ptr multi-pass channel offs
    *((volatile uint32_t *)0x50900590) = 0x00000920; // Layer control
    *((volatile uint32_t *)0x50900a10) = 0x0001f8f0; // Layer control 2
    *((volatile uint32_t *)0x50900610) = 0x00001ff8; // Mask offset and count
    *((volatile uint32_t *)0x50900110) = 0x00000100; // 1D
    *((volatile uint32_t *)0x50900790) = 0x00022000; // Post processing register
    *((volatile uint32_t *)0x50900710) = 0xffffffff; // Mask and processor enables

    // Layer 0 quadrant 3
    *((volatile uint32_t *)0x50d00310) = 0x00001000; // SRAM write ptr
    *((volatile uint32_t *)0x50d00390) = 0x00000001; // Write ptr time slot offs
    *((volatile uint32_t *)0x50d00410) = 0x00002000; // Write ptr mask offs
    *((volatile uint32_t *)0x50d00490) = 0x00000001; // Write ptr multi-pass channel offs
    *((volatile uint32_t *)0x50d00590) = 0x00000920; // Layer control
    *((volatile uint32_t *)0x50d00a10) = 0x0001f8f0; // Layer control 2
    *((volatile uint32_t *)0x50d00610) = 0x00001ff8; // Mask offset and count
    *((volatile uint32_t *)0x50d00110) = 0x00000100; // 1D
    *((volatile uint32_t *)0x50d00790) = 0x00022000; // Post processing register
    *((volatile uint32_t *)0x50d00710) = 0xffffffff; // Mask and processor enables

    return CNN_OK;
}

int cnn_3_start(void)
{
    cnn_time = 0;

    *((volatile uint32_t *)0x50100000) = 0x00100808; // Enable quadrant 0
    *((volatile uint32_t *)0x50500000) = 0x00100809; // Enable quadrant 1
    *((volatile uint32_t *)0x50900000) = 0x00100809; // Enable quadrant 2
    *((volatile uint32_t *)0x50d00000) = 0x00100809; // Enable quadrant 3

#ifdef CNN_INFERENCE_TIMER
    MXC_TMR_SW_Start(CNN_INFERENCE_TIMER);
#endif

    CNN_START; // Allow capture of processing time
    *((volatile uint32_t *)0x50100000) = 0x00100009; // Master enable quadrant 0

    return CNN_OK;
}

int cnn_3_unload(uint32_t *out_buf32)
{
    uint8_t *out_buf = (uint8_t *)out_buf32;
    uint32_t val;
    volatile uint32_t *addr;
    int i;

    // Custom unload for this network, layer 0: 8-bit data, shape: (1024, 1, 1)
    addr = (volatile uint32_t *)0x50404000;
    for (i = 0; i < 4; i++) {
        val = *addr;
        addr += 0x2000;
        *out_buf++ = val & 0xff;
        *out_buf++ = (val >> 8) & 0xff;
        *out_buf++ = (val >> 16) & 0xff;
        *out_buf++ = (val >> 24) & 0xff;
    }
    addr = (volatile uint32_t *)0x50804000;
    for (i = 0; i < 4; i++) {
        val = *addr;
        addr += 0x2000;
        *out_buf++ = val & 0xff;
        *out_buf++ = (val >> 8) & 0xff;
        *out_buf++ = (val >> 16) & 0xff;
        *out_buf++ = (val >> 24) & 0xff;
    }
    addr = (volatile uint32_t *)0x50c04000;
    for (i = 0; i < 4; i++) {
        val = *addr;
        addr += 0x2000;
        *out_buf++ = val & 0xff;
        *out_buf++ = (val >> 8) & 0xff;
        *out_buf++ = (val >> 16) & 0xff;
        *out_buf++ = (val >> 24) & 0xff;
    }
    addr = (volatile uint32_t *)0x51004000;
    for (i = 0; i < 4; i++) {
        val = *addr;
        addr += 0x2000;
        *out_buf++ = val & 0xff;
        *out_buf++ = (val >> 8) & 0xff;
        *out_buf++ = (val >> 16) & 0xff;
        *out_buf++ = (val >> 24) & 0xff;
    }
    addr = (volatile uint32_t *)0x50404004;
    for (i = 0; i < 4; i++) {
        val = *addr;
        addr += 0x2000;
        *out_buf++ = val & 0xff;
        *out_buf++ = (val >> 8) & 0xff;
        *out_buf++ = (val >> 16) & 0xff;
        *out_buf++ = (val >> 24) & 0xff;
    }
    addr = (volatile uint32_t *)0x50804004;
    for (i = 0; i < 4; i++) {
        val = *addr;
        addr += 0x2000;
        *out_buf++ = val & 0xff;
        *out_buf++ = (val >> 8) & 0xff;
        *out_buf++ = (val >> 16) & 0xff;
        *out_buf++ = (val >> 24) & 0xff;
    }
    addr = (volatile uint32_t *)0x50c04004;
    for (i = 0; i < 4; i++) {
        val = *addr;
        addr += 0x2000;
        *out_buf++ = val & 0xff;
        *out_buf++ = (val >> 8) & 0xff;
        *out_buf++ = (val >> 16) & 0xff;
        *out_buf++ = (val >> 24) & 0xff;
    }
    addr = (volatile uint32_t *)0x51004004;
    for (i = 0; i < 4; i++) {
        val = *addr;
        addr += 0x2000;
        *out_buf++ = val & 0xff;
        *out_buf++ = (val >> 8) & 0xff;
        *out_buf++ = (val >> 16) & 0xff;
        *out_buf++ = (val >> 24) & 0xff;
    }
    addr = (volatile uint32_t *)0x50404008;
    for (i = 0; i < 4; i++) {
        val = *addr;
        addr += 0x2000;
        *out_buf++ = val & 0xff;
        *out_buf++ = (val >> 8) & 0xff;
        *out_buf++ = (val >> 16) & 0xff;
        *out_buf++ = (val >> 24) & 0xff;
    }
    addr = (volatile uint32_t *)0x50804008;
    for (i = 0; i < 4; i++) {
        val = *addr;
        addr += 0x2000;
        *out_buf++ = val & 0xff;
        *out_buf++ = (val >> 8) & 0xff;
        *out_buf++ = (val >> 16) & 0xff;
        *out_buf++ = (val >> 24) & 0xff;
    }
    addr = (volatile uint32_t *)0x50c04008;
    for (i = 0; i < 4; i++) {
        val = *addr;
        addr += 0x2000;
        *out_buf++ = val & 0xff;
        *out_buf++ = (val >> 8) & 0xff;
        *out_buf++ = (val >> 16) & 0xff;
        *out_buf++ = (val >> 24) & 0xff;
    }
    addr = (volatile uint32_t *)0x51004008;
    for (i = 0; i < 4; i++) {
        val = *addr;
        addr += 0x2000;
        *out_buf++ = val & 0xff;
        *out_buf++ = (val >> 8) & 0xff;
        *out_buf++ = (val >> 16) & 0xff;
        *out_buf++ = (val >> 24) & 0xff;
    }
    addr = (volatile uint32_t *)0x5040400c;
    for (i = 0; i < 4; i++) {
        val = *addr;
        addr += 0x2000;
        *out_buf++ = val & 0xff;
        *out_buf++ = (val >> 8) & 0xff;
        *out_buf++ = (val >> 16) & 0xff;
        *out_buf++ = (val >> 24) & 0xff;
    }
    addr = (volatile uint32_t *)0x5080400c;
    for (i = 0; i < 4; i++) {
        val = *addr;
        addr += 0x2000;
        *out_buf++ = val & 0xff;
        *out_buf++ = (val >> 8) & 0xff;
        *out_buf++ = (val >> 16) & 0xff;
        *out_buf++ = (val >> 24) & 0xff;
    }
    addr = (volatile uint32_t *)0x50c0400c;
    for (i = 0; i < 4; i++) {
        val = *addr;
        addr += 0x2000;
        *out_buf++ = val & 0xff;
        *out_buf++ = (val >> 8) & 0xff;
        *out_buf++ = (val >> 16) & 0xff;
        *out_buf++ = (val >> 24) & 0xff;
    }
    addr = (volatile uint32_t *)0x5100400c;
    for (i = 0; i < 4; i++) {
        val = *addr;
        addr += 0x2000;
        *out_buf++ = val & 0xff;
        *out_buf++ = (val >> 8) & 0xff;
        *out_buf++ = (val >> 16) & 0xff;
        *out_buf++ = (val >> 24) & 0xff;
    }
    addr = (volatile uint32_t *)0x50404010;
    for (i = 0; i < 4; i++) {
        val = *addr;
        addr += 0x2000;
        *out_buf++ = val & 0xff;
        *out_buf++ = (val >> 8) & 0xff;
        *out_buf++ = (val >> 16) & 0xff;
        *out_buf++ = (val >> 24) & 0xff;
    }
    addr = (volatile uint32_t *)0x50804010;
    for (i = 0; i < 4; i++) {
        val = *addr;
        addr += 0x2000;
        *out_buf++ = val & 0xff;
        *out_buf++ = (val >> 8) & 0xff;
        *out_buf++ = (val >> 16) & 0xff;
        *out_buf++ = (val >> 24) & 0xff;
    }
    addr = (volatile uint32_t *)0x50c04010;
    for (i = 0; i < 4; i++) {
        val = *addr;
        addr += 0x2000;
        *out_buf++ = val & 0xff;
        *out_buf++ = (val >> 8) & 0xff;
        *out_buf++ = (val >> 16) & 0xff;
        *out_buf++ = (val >> 24) & 0xff;
    }
    addr = (volatile uint32_t *)0x51004010;
    for (i = 0; i < 4; i++) {
        val = *addr;
        addr += 0x2000;
        *out_buf++ = val & 0xff;
        *out_buf++ = (val >> 8) & 0xff;
        *out_buf++ = (val >> 16) & 0xff;
        *out_buf++ = (val >> 24) & 0xff;
    }
    addr = (volatile uint32_t *)0x50404014;
    for (i = 0; i < 4; i++) {
        val = *addr;
        addr += 0x2000;
        *out_buf++ = val & 0xff;
        *out_buf++ = (val >> 8) & 0xff;
        *out_buf++ = (val >> 16) & 0xff;
        *out_buf++ = (val >> 24) & 0xff;
    }
    addr = (volatile uint32_t *)0x50804014;
    for (i = 0; i < 4; i++) {
        val = *addr;
        addr += 0x2000;
        *out_buf++ = val & 0xff;
        *out_buf++ = (val >> 8) & 0xff;
        *out_buf++ = (val >> 16) & 0xff;
        *out_buf++ = (val >> 24) & 0xff;
    }
    addr = (volatile uint32_t *)0x50c04014;
    for (i = 0; i < 4; i++) {
        val = *addr;
        addr += 0x2000;
        *out_buf++ = val & 0xff;
        *out_buf++ = (val >> 8) & 0xff;
        *out_buf++ = (val >> 16) & 0xff;
        *out_buf++ = (val >> 24) & 0xff;
    }
    addr = (volatile uint32_t *)0x51004014;
    for (i = 0; i < 4; i++) {
        val = *addr;
        addr += 0x2000;
        *out_buf++ = val & 0xff;
        *out_buf++ = (val >> 8) & 0xff;
        *out_buf++ = (val >> 16) & 0xff;
        *out_buf++ = (val >> 24) & 0xff;
    }
    addr = (volatile uint32_t *)0x50404018;
    for (i = 0; i < 4; i++) {
        val = *addr;
        addr += 0x2000;
        *out_buf++ = val & 0xff;
        *out_buf++ = (val >> 8) & 0xff;
        *out_buf++ = (val >> 16) & 0xff;
        *out_buf++ = (val >> 24) & 0xff;
    }
    addr = (volatile uint32_t *)0x50804018;
    for (i = 0; i < 4; i++) {
        val = *addr;
        addr += 0x2000;
        *out_buf++ = val & 0xff;
        *out_buf++ = (val >> 8) & 0xff;
        *out_buf++ = (val >> 16) & 0xff;
        *out_buf++ = (val >> 24) & 0xff;
    }
    addr = (volatile uint32_t *)0x50c04018;
    for (i = 0; i < 4; i++) {
        val = *addr;
        addr += 0x2000;
        *out_buf++ = val & 0xff;
        *out_buf++ = (val >> 8) & 0xff;
        *out_buf++ = (val >> 16) & 0xff;
        *out_buf++ = (val >> 24) & 0xff;
    }
    addr = (volatile uint32_t *)0x51004018;
    for (i = 0; i < 4; i++) {
        val = *addr;
        addr += 0x2000;
        *out_buf++ = val & 0xff;
        *out_buf++ = (val >> 8) & 0xff;
        *out_buf++ = (val >> 16) & 0xff;
        *out_buf++ = (val >> 24) & 0xff;
    }
    addr = (volatile uint32_t *)0x5040401c;
    for (i = 0; i < 4; i++) {
        val = *addr;
        addr += 0x2000;
        *out_buf++ = val & 0xff;
        *out_buf++ = (val >> 8) & 0xff;
        *out_buf++ = (val >> 16) & 0xff;
        *out_buf++ = (val >> 24) & 0xff;
    }
    addr = (volatile uint32_t *)0x5080401c;
    for (i = 0; i < 4; i++) {
        val = *addr;
        addr += 0x2000;
        *out_buf++ = val & 0xff;
        *out_buf++ = (val >> 8) & 0xff;
        *out_buf++ = (val >> 16) & 0xff;
        *out_buf++ = (val >> 24) & 0xff;
    }
    addr = (volatile uint32_t *)0x50c0401c;
    for (i = 0; i < 4; i++) {
        val = *addr;
        addr += 0x2000;
        *out_buf++ = val & 0xff;
        *out_buf++ = (val >> 8) & 0xff;
        *out_buf++ = (val >> 16) & 0xff;
        *out_buf++ = (val >> 24) & 0xff;
    }
    addr = (volatile uint32_t *)0x5100401c;
    for (i = 0; i < 4; i++) {
        val = *addr;
        addr += 0x2000;
        *out_buf++ = val & 0xff;
        *out_buf++ = (val >> 8) & 0xff;
        *out_buf++ = (val >> 16) & 0xff;
        *out_buf++ = (val >> 24) & 0xff;
    }
    addr = (volatile uint32_t *)0x50404020;
    for (i = 0; i < 4; i++) {
        val = *addr;
        addr += 0x2000;
        *out_buf++ = val & 0xff;
        *out_buf++ = (val >> 8) & 0xff;
        *out_buf++ = (val >> 16) & 0xff;
        *out_buf++ = (val >> 24) & 0xff;
    }
    addr = (volatile uint32_t *)0x50804020;
    for (i = 0; i < 4; i++) {
        val = *addr;
        addr += 0x2000;
        *out_buf++ = val & 0xff;
        *out_buf++ = (val >> 8) & 0xff;
        *out_buf++ = (val >> 16) & 0xff;
        *out_buf++ = (val >> 24) & 0xff;
    }
    addr = (volatile uint32_t *)0x50c04020;
    for (i = 0; i < 4; i++) {
        val = *addr;
        addr += 0x2000;
        *out_buf++ = val & 0xff;
        *out_buf++ = (val >> 8) & 0xff;
        *out_buf++ = (val >> 16) & 0xff;
        *out_buf++ = (val >> 24) & 0xff;
    }
    addr = (volatile uint32_t *)0x51004020;
    for (i = 0; i < 4; i++) {
        val = *addr;
        addr += 0x2000;
        *out_buf++ = val & 0xff;
        *out_buf++ = (val >> 8) & 0xff;
        *out_buf++ = (val >> 16) & 0xff;
        *out_buf++ = (val >> 24) & 0xff;
    }
    addr = (volatile uint32_t *)0x50404024;
    for (i = 0; i < 4; i++) {
        val = *addr;
        addr += 0x2000;
        *out_buf++ = val & 0xff;
        *out_buf++ = (val >> 8) & 0xff;
        *out_buf++ = (val >> 16) & 0xff;
        *out_buf++ = (val >> 24) & 0xff;
    }
    addr = (volatile uint32_t *)0x50804024;
    for (i = 0; i < 4; i++) {
        val = *addr;
        addr += 0x2000;
        *out_buf++ = val & 0xff;
        *out_buf++ = (val >> 8) & 0xff;
        *out_buf++ = (val >> 16) & 0xff;
        *out_buf++ = (val >> 24) & 0xff;
    }
    addr = (volatile uint32_t *)0x50c04024;
    for (i = 0; i < 4; i++) {
        val = *addr;
        addr += 0x2000;
        *out_buf++ = val & 0xff;
        *out_buf++ = (val >> 8) & 0xff;
        *out_buf++ = (val >> 16) & 0xff;
        *out_buf++ = (val >> 24) & 0xff;
    }
    addr = (volatile uint32_t *)0x51004024;
    for (i = 0; i < 4; i++) {
        val = *addr;
        addr += 0x2000;
        *out_buf++ = val & 0xff;
        *out_buf++ = (val >> 8) & 0xff;
        *out_buf++ = (val >> 16) & 0xff;
        *out_buf++ = (val >> 24) & 0xff;
    }
    addr = (volatile uint32_t *)0x50404028;
    for (i = 0; i < 4; i++) {
        val = *addr;
        addr += 0x2000;
        *out_buf++ = val & 0xff;
        *out_buf++ = (val >> 8) & 0xff;
        *out_buf++ = (val >> 16) & 0xff;
        *out_buf++ = (val >> 24) & 0xff;
    }
    addr = (volatile uint32_t *)0x50804028;
    for (i = 0; i < 4; i++) {
        val = *addr;
        addr += 0x2000;
        *out_buf++ = val & 0xff;
        *out_buf++ = (val >> 8) & 0xff;
        *out_buf++ = (val >> 16) & 0xff;
        *out_buf++ = (val >> 24) & 0xff;
    }
    addr = (volatile uint32_t *)0x50c04028;
    for (i = 0; i < 4; i++) {
        val = *addr;
        addr += 0x2000;
        *out_buf++ = val & 0xff;
        *out_buf++ = (val >> 8) & 0xff;
        *out_buf++ = (val >> 16) & 0xff;
        *out_buf++ = (val >> 24) & 0xff;
    }
    addr = (volatile uint32_t *)0x51004028;
    for (i = 0; i < 4; i++) {
        val = *addr;
        addr += 0x2000;
        *out_buf++ = val & 0xff;
        *out_buf++ = (val >> 8) & 0xff;
        *out_buf++ = (val >> 16) & 0xff;
        *out_buf++ = (val >> 24) & 0xff;
    }
    addr = (volatile uint32_t *)0x5040402c;
    for (i = 0; i < 4; i++) {
        val = *addr;
        addr += 0x2000;
        *out_buf++ = val & 0xff;
        *out_buf++ = (val >> 8) & 0xff;
        *out_buf++ = (val >> 16) & 0xff;
        *out_buf++ = (val >> 24) & 0xff;
    }
    addr = (volatile uint32_t *)0x5080402c;
    for (i = 0; i < 4; i++) {
        val = *addr;
        addr += 0x2000;
        *out_buf++ = val & 0xff;
        *out_buf++ = (val >> 8) & 0xff;
        *out_buf++ = (val >> 16) & 0xff;
        *out_buf++ = (val >> 24) & 0xff;
    }
    addr = (volatile uint32_t *)0x50c0402c;
    for (i = 0; i < 4; i++) {
        val = *addr;
        addr += 0x2000;
        *out_buf++ = val & 0xff;
        *out_buf++ = (val >> 8) & 0xff;
        *out_buf++ = (val >> 16) & 0xff;
        *out_buf++ = (val >> 24) & 0xff;
    }
    addr = (volatile uint32_t *)0x5100402c;
    for (i = 0; i < 4; i++) {
        val = *addr;
        addr += 0x2000;
        *out_buf++ = val & 0xff;
        *out_buf++ = (val >> 8) & 0xff;
        *out_buf++ = (val >> 16) & 0xff;
        *out_buf++ = (val >> 24) & 0xff;
    }
    addr = (volatile uint32_t *)0x50404030;
    for (i = 0; i < 4; i++) {
        val = *addr;
        addr += 0x2000;
        *out_buf++ = val & 0xff;
        *out_buf++ = (val >> 8) & 0xff;
        *out_buf++ = (val >> 16) & 0xff;
        *out_buf++ = (val >> 24) & 0xff;
    }
    addr = (volatile uint32_t *)0x50804030;
    for (i = 0; i < 4; i++) {
        val = *addr;
        addr += 0x2000;
        *out_buf++ = val & 0xff;
        *out_buf++ = (val >> 8) & 0xff;
        *out_buf++ = (val >> 16) & 0xff;
        *out_buf++ = (val >> 24) & 0xff;
    }
    addr = (volatile uint32_t *)0x50c04030;
    for (i = 0; i < 4; i++) {
        val = *addr;
        addr += 0x2000;
        *out_buf++ = val & 0xff;
        *out_buf++ = (val >> 8) & 0xff;
        *out_buf++ = (val >> 16) & 0xff;
        *out_buf++ = (val >> 24) & 0xff;
    }
    addr = (volatile uint32_t *)0x51004030;
    for (i = 0; i < 4; i++) {
        val = *addr;
        addr += 0x2000;
        *out_buf++ = val & 0xff;
        *out_buf++ = (val >> 8) & 0xff;
        *out_buf++ = (val >> 16) & 0xff;
        *out_buf++ = (val >> 24) & 0xff;
    }
    addr = (volatile uint32_t *)0x50404034;
    for (i = 0; i < 4; i++) {
        val = *addr;
        addr += 0x2000;
        *out_buf++ = val & 0xff;
        *out_buf++ = (val >> 8) & 0xff;
        *out_buf++ = (val >> 16) & 0xff;
        *out_buf++ = (val >> 24) & 0xff;
    }
    addr = (volatile uint32_t *)0x50804034;
    for (i = 0; i < 4; i++) {
        val = *addr;
        addr += 0x2000;
        *out_buf++ = val & 0xff;
        *out_buf++ = (val >> 8) & 0xff;
        *out_buf++ = (val >> 16) & 0xff;
        *out_buf++ = (val >> 24) & 0xff;
    }
    addr = (volatile uint32_t *)0x50c04034;
    for (i = 0; i < 4; i++) {
        val = *addr;
        addr += 0x2000;
        *out_buf++ = val & 0xff;
        *out_buf++ = (val >> 8) & 0xff;
        *out_buf++ = (val >> 16) & 0xff;
        *out_buf++ = (val >> 24) & 0xff;
    }
    addr = (volatile uint32_t *)0x51004034;
    for (i = 0; i < 4; i++) {
        val = *addr;
        addr += 0x2000;
        *out_buf++ = val & 0xff;
        *out_buf++ = (val >> 8) & 0xff;
        *out_buf++ = (val >> 16) & 0xff;
        *out_buf++ = (val >> 24) & 0xff;
    }
    addr = (volatile uint32_t *)0x50404038;
    for (i = 0; i < 4; i++) {
        val = *addr;
        addr += 0x2000;
        *out_buf++ = val & 0xff;
        *out_buf++ = (val >> 8) & 0xff;
        *out_buf++ = (val >> 16) & 0xff;
        *out_buf++ = (val >> 24) & 0xff;
    }
    addr = (volatile uint32_t *)0x50804038;
    for (i = 0; i < 4; i++) {
        val = *addr;
        addr += 0x2000;
        *out_buf++ = val & 0xff;
        *out_buf++ = (val >> 8) & 0xff;
        *out_buf++ = (val >> 16) & 0xff;
        *out_buf++ = (val >> 24) & 0xff;
    }
    addr = (volatile uint32_t *)0x50c04038;
    for (i = 0; i < 4; i++) {
        val = *addr;
        addr += 0x2000;
        *out_buf++ = val & 0xff;
        *out_buf++ = (val >> 8) & 0xff;
        *out_buf++ = (val >> 16) & 0xff;
        *out_buf++ = (val >> 24) & 0xff;
    }
    addr = (volatile uint32_t *)0x51004038;
    for (i = 0; i < 4; i++) {
        val = *addr;
        addr += 0x2000;
        *out_buf++ = val & 0xff;
        *out_buf++ = (val >> 8) & 0xff;
        *out_buf++ = (val >> 16) & 0xff;
        *out_buf++ = (val >> 24) & 0xff;
    }
    addr = (volatile uint32_t *)0x5040403c;
    for (i = 0; i < 4; i++) {
        val = *addr;
        addr += 0x2000;
        *out_buf++ = val & 0xff;
        *out_buf++ = (val >> 8) & 0xff;
        *out_buf++ = (val >> 16) & 0xff;
        *out_buf++ = (val >> 24) & 0xff;
    }
    addr = (volatile uint32_t *)0x5080403c;
    for (i = 0; i < 4; i++) {
        val = *addr;
        addr += 0x2000;
        *out_buf++ = val & 0xff;
        *out_buf++ = (val >> 8) & 0xff;
        *out_buf++ = (val >> 16) & 0xff;
        *out_buf++ = (val >> 24) & 0xff;
    }
    addr = (volatile uint32_t *)0x50c0403c;
    for (i = 0; i < 4; i++) {
        val = *addr;
        addr += 0x2000;
        *out_buf++ = val & 0xff;
        *out_buf++ = (val >> 8) & 0xff;
        *out_buf++ = (val >> 16) & 0xff;
        *out_buf++ = (val >> 24) & 0xff;
    }
    addr = (volatile uint32_t *)0x5100403c;
    for (i = 0; i < 4; i++) {
        val = *addr;
        addr += 0x2000;
        *out_buf++ = val & 0xff;
        *out_buf++ = (val >> 8) & 0xff;
        *out_buf++ = (val >> 16) & 0xff;
        *out_buf++ = (val >> 24) & 0xff;
    }

    return CNN_OK;
}
