/**
 * @file        main.c
 * @brief       GPIO Example
 * @details
 */

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

/***** Includes *****/
#include <stdio.h>
#include <string.h>
#include "mxc_device.h"
#include "board.h"
#include "gpio.h"
#include "example_config.h"

/***** Definitions *****/

/***** Globals *****/

/***** Functions *****/
static void gpio_isr(void *cbdata)
{
    mxc_gpio_cfg_t *cfg = cbdata;
    MXC_GPIO_OutToggle(cfg->port, cfg->mask);
}

int main(void)
{
    mxc_gpio_cfg_t gpio_in;
    mxc_gpio_cfg_t gpio_out;
    mxc_gpio_cfg_t gpio_interrupt;
    mxc_gpio_cfg_t gpio_interrupt_status;

    printf("\n\n****** GPIO Example ******\n");
    printf("Demonstrates GPIO get/set and interrupt usage\n");
#if defined(BOARD_FTHR)
    printf("1. This example reads P1.10 (SW1) and outputs the same state onto P0.29 (LED0).\n");
    printf("2. An interrupt is set up on P0.16. When interrupt occurs P0.30 (LED1) toggles.\n\n");
#elif defined(BOARD_FTHR2)
    printf("1. This example reads P0.24 (SW2) and outputs the same state onto P0.29 (LED0).\n");
    printf("2. An interrupt is set up on P0.28 (SW3). When interrupt occurs P0.30 (LED1) "
           "toggles.\n\n");
#else
    printf("1. This example reads P1.06 (SW2) and outputs the same state onto P1.14 (LED0).\n");
    printf("2. An interrupt is set up on P1.07 (SW3). When interrupt occurs P1.15 (LED1) "
           "toggles.\n\n");
#endif

    /* Setup interrupt status pin as an output so we can toggle it on each interrupt. */
    gpio_interrupt_status.port = MXC_GPIO_PORT_INTERRUPT_STATUS;
    gpio_interrupt_status.mask = MXC_GPIO_PIN_INTERRUPT_STATUS;
    gpio_interrupt_status.pad = MXC_GPIO_PAD_NONE;
    gpio_interrupt_status.func = MXC_GPIO_FUNC_OUT;
    gpio_interrupt_status.vssel = MXC_GPIO_VSSEL_VDDIO;
    gpio_interrupt_status.drvstr = MXC_GPIO_DRVSTR_0;
    MXC_GPIO_Config(&gpio_interrupt_status);

    /*
  *   Set up interrupt on MXC_GPIO_PORT_INTERRUPT_IN.
  *   Switch on EV kit is open when non-pressed, and grounded when pressed.  Use an internal pull-up so pin
  *     reads high when button is not pressed.
  */
    gpio_interrupt.port = MXC_GPIO_PORT_INTERRUPT_IN;
    gpio_interrupt.mask = MXC_GPIO_PIN_INTERRUPT_IN;
    gpio_interrupt.pad = MXC_GPIO_PAD_PULL_UP;
    gpio_interrupt.func = MXC_GPIO_FUNC_IN;
    gpio_interrupt.vssel = MXC_GPIO_VSSEL_VDDIO;
    MXC_GPIO_RegisterCallback(&gpio_interrupt, gpio_isr, &gpio_interrupt_status);
    MXC_GPIO_IntConfig(&gpio_interrupt, MXC_GPIO_INT_FALLING);
    MXC_GPIO_EnableInt(gpio_interrupt.port, gpio_interrupt.mask);
    NVIC_EnableIRQ(MXC_GPIO_GET_IRQ(MXC_GPIO_GET_IDX(MXC_GPIO_PORT_INTERRUPT_IN)));

    /*
  *   Setup input pin.
  *   Switch on EV kit is open when non-pressed, and grounded when pressed.  Use an internal pull-up so pin
  *	    reads high when button is not pressed.
  */
    gpio_in.port = MXC_GPIO_PORT_IN;
    gpio_in.mask = MXC_GPIO_PIN_IN;
    gpio_in.pad = MXC_GPIO_PAD_PULL_UP;
    gpio_in.func = MXC_GPIO_FUNC_IN;
    gpio_in.vssel = MXC_GPIO_VSSEL_VDDIO;
    gpio_in.drvstr = MXC_GPIO_DRVSTR_0;
    MXC_GPIO_Config(&gpio_in);

    /* Setup output pin. */
    gpio_out.port = MXC_GPIO_PORT_OUT;
    gpio_out.mask = MXC_GPIO_PIN_OUT;
    gpio_out.pad = MXC_GPIO_PAD_NONE;
    gpio_out.func = MXC_GPIO_FUNC_OUT;
    gpio_out.vssel = MXC_GPIO_VSSEL_VDDIO;
    gpio_out.drvstr = MXC_GPIO_DRVSTR_0;
    MXC_GPIO_Config(&gpio_out);

    while (1) {
        /* Read state of the input pin. */
        if (MXC_GPIO_InGet(gpio_in.port, gpio_in.mask)) {
            /* Input pin was high, set the output pin. */
            MXC_GPIO_OutSet(gpio_out.port, gpio_out.mask);
        } else {
            /* Input pin was low, clear the output pin. */
            MXC_GPIO_OutClr(gpio_out.port, gpio_out.mask);
        }
    }

    return 0;
}
