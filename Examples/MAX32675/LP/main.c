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

/*
 * @file    main.c
 * @brief   Demonstrates the various low power modes.
 *
 * @details Iterates through the various low power modes, using either the RTC
 *          alarm or a GPIO to wake from each.  #defines determine which wakeup
 *          source to use.  Once the code is running, you can measure the
 *          current used on the VCORE rail.
 *
 *          The power states shown are:
 *            1. Active mode power with all clocks on
 *            2. Active mode power with peripheral clocks disabled
 *            3. Active mode power with unused RAMs in light sleep mode
 *            4. Active mode power with unused RAMS shut down
 *            5. SLEEP mode
 *            6. BACKGROUND mode
 *            7. DEEPSLEEP mode
 *            8. BACKUP mode
 */

#include <stdio.h>
#include <stdint.h>
#include "mxc_device.h"
#include "mxc_errors.h"
#include "pb.h"
#include "led.h"
#include "lp.h"
#include "icc.h"
#include "rtc.h"
#include "uart.h"
#include "nvic_table.h"

#define DELAY_IN_SEC 2
#define USE_CONSOLE 1

#define DO_SLEEP 1
#define DO_DEEPSLEEP 1
#define DO_BACKUP 0
#define DO_STORAGE 0

#if (DO_BACKUP && DO_STORAGE)
#error "You must select either DO_BACKUP or DO_STORAGE or neither, not both."
#endif

#if USE_CONSOLE
#define PRINT(...) fprintf(stdout, __VA_ARGS__)
#else
#define PRINT(...)
#endif

// *****************************************************************************
volatile int buttonPressed;
void buttonHandler(void *pb)
{
    buttonPressed = 1;
}

void setTrigger(int waitForTrigger)
{
    int tmp;

    buttonPressed = 0;

    if (waitForTrigger) {
        while (!buttonPressed) {}
    }

    // Debounce the button press.
    for (tmp = 0; tmp < 0x80000; tmp++) {
        __NOP();
    }

    // Wait for serial transactions to complete.
#if USE_CONSOLE

    while (MXC_UART_ReadyForSleep(MXC_UART_GET_UART(CONSOLE_UART)) != E_NO_ERROR) {}

#endif // USE_CONSOLE
}

void configure_gpio(void)
{
    //Set GPIOs to output mode except PB0 and UART0 pins
    MXC_GPIO0->en0 |= 0xFFFFFCFFUL;
    MXC_GPIO0->outen |= 0xFFFFFCFFUL;

    MXC_GPIO1->en0 |= 0xFFFFF7FFUL;
    MXC_GPIO1->outen |= 0xFFFFF7FFUL;

    // Pull down all the GPIO pins except PB0 and UART0
    MXC_GPIO0->padctrl0 &= ~0xFFFFFFFFUL;
    MXC_GPIO0->padctrl1 |= 0xFFFFFFFFUL;
    MXC_GPIO0->ps &= ~0xFFFFFFFFUL;
    MXC_GPIO1->padctrl0 &= ~0xFFFFF7FFUL;
    MXC_GPIO1->padctrl1 |= 0xFFFFF7FFUL;
    MXC_GPIO1->ps &= ~0xFFFFF7FFUL;
}

int main(void)
{
    PRINT("****Low Power Mode Example****\n\n");

    PRINT("This code cycles through the MAX32675 power modes, using a push button 0 (SW1) to exit "
          "from each mode and enter the next.\n\n");
    PB_RegisterCallback(0, buttonHandler);

    //Pull down all GPIOs except PB0 and UART0 pins
    configure_gpio();

    PRINT("Running in ACTIVE mode.\n");
#if !USE_CONSOLE
    MXC_SYS_ClockDisable(MXC_SYS_PERIPH_CLOCK_UART0);
#endif // !USE_CONSOLE
    setTrigger(1);

    MXC_LP_ROMLightSleepEnable();
    MXC_LP_ICache0LightSleepEnable();
    MXC_LP_SysRam3LightSleepEnable();
    MXC_LP_SysRam2LightSleepEnable();
    MXC_LP_SysRam1LightSleepDisable();
    MXC_LP_SysRam0LightSleepDisable(); // Global variables are in RAM0 and RAM1

    PRINT("All unused RAMs placed in LIGHT SLEEP mode.\n");
    setTrigger(1);

    MXC_LP_SysRam3Shutdown();
    MXC_LP_SysRam2Shutdown();
    MXC_LP_SysRam1PowerUp();
    MXC_LP_SysRam0PowerUp(); // Global variables are in RAM0 and RAM1

    PRINT("All unused RAMs shutdown.\n");
    setTrigger(1);

    MXC_LP_EnableGPIOWakeup((mxc_gpio_cfg_t *)&pb_pin[0]);
    MXC_GPIO_SetWakeEn(pb_pin[0].port, pb_pin[0].mask);

    while (1) {
#if DO_SLEEP
        PRINT("Entering SLEEP mode.\n");
        setTrigger(0);
        MXC_LP_EnterSleepMode();
        PRINT("Waking up from SLEEP mode.\n");

#endif // DO_SLEEP
#if DO_DEEPSLEEP
        PRINT("Entering DEEPSLEEP mode.\n");
        setTrigger(0);
        MXC_LP_EnterDeepSleepMode();
        PRINT("Waking up from DEEPSLEEP mode.\n");
#endif // DO_DEEPSLEEP

#if DO_BACKUP
        PRINT("Entering BACKUP mode.\n");
        setTrigger(0);
        MXC_LP_EnterBackupMode();
#endif // DO_BACKUP

#if DO_STORAGE
        PRINT("Entering Storage mode.\n");
        setTrigger(0);
        MXC_LP_EnterStorageMode();
#endif // DO_STORAGE
    }
}
