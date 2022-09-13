/******************************************************************************
 * Copyright (C) 2022 Maxim Integrated Products, Inc., All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL MAXIM INTEGRATED BE LIABLE FOR ANY CLAIM, DAMAGES
 * OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * Except as contained in this notice, the name of Maxim Integrated
 * Products, Inc. shall not be used except as stated in the Maxim Integrated
 * Products, Inc. Branding Policy.
 *
 * The mere transfer of this software does not imply any licenses
 * of trade secrets, proprietary technology, copyrights, patents,
 * trademarks, maskwork rights, or any other form of intellectual
 * property whatsoever. Maxim Integrated Products, Inc. retains all
 * ownership rights.
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
 *            3. Active mode power with unused RAMs shut down
 *            4. SLEEP mode
 *            5. LPM mode
 *            6. UPM mode
 *            7. BACKUP mode
 *            8. STANDBY mode
 */

#include <stdio.h>
#include <stdint.h>
#include "mxc_delay.h"
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

#define USE_BUTTON 0
#define USE_ALARM 1

#define DO_SLEEP 1
#define DO_LPM 1
#define DO_UPM 0
#define DO_BACKUP 0
#define DO_STANDBY 0

#if (!(USE_BUTTON || USE_ALARM))
#error "You must set either USE_BUTTON or USE_ALARM to 1."
#endif
#if (USE_BUTTON && USE_ALARM)
#error "You must select either USE_BUTTON or USE_ALARM, not both."
#endif

#if (DO_BACKUP && DO_STANDBY)
#error "You must select either DO_BACKUP or DO_STORAGE or neither, not both."
#endif

// *****************************************************************************

#if USE_ALARM
volatile int alarmed;
void alarmHandler(void)
{
    int flags = MXC_RTC->ctrl;
    alarmed = 1;

    if ((flags & MXC_F_RTC_CTRL_SSEC_ALARM) >> MXC_F_RTC_CTRL_SSEC_ALARM_POS) {
        MXC_RTC->ctrl &= ~(MXC_F_RTC_CTRL_SSEC_ALARM);
    }

    if ((flags & MXC_F_RTC_CTRL_TOD_ALARM) >> MXC_F_RTC_CTRL_TOD_ALARM_POS) {
        MXC_RTC->ctrl &= ~(MXC_F_RTC_CTRL_TOD_ALARM);
    }
}

void setTrigger(int waitForTrigger)
{
    alarmed = 0;

    while (MXC_RTC_Init(0, 0) == E_BUSY) {}

    while (MXC_RTC_DisableInt(MXC_F_RTC_CTRL_TOD_ALARM_IE) == E_BUSY) {}

    while (MXC_RTC_SetTimeofdayAlarm(DELAY_IN_SEC) == E_BUSY) {}

    while (MXC_RTC_EnableInt(MXC_F_RTC_CTRL_TOD_ALARM_IE) == E_BUSY) {}

    while (MXC_RTC_Start() == E_BUSY) {}

    if (waitForTrigger) {
        while (!alarmed) {}
    }

    // Wait for serial transactions to complete.
#if USE_CONSOLE

    while (MXC_UART_ReadyForSleep(MXC_UART_GET_UART(CONSOLE_UART)) != E_NO_ERROR) {}

#endif // USE_CONSOLE
}
#endif // USE_ALARM

#if USE_BUTTON
volatile int buttonPressed;
void buttonHandler(void *pb)
{
    MXC_Delay(MXC_DELAY_MSEC(250));
    buttonPressed = 1;
}

void setTrigger(int waitForTrigger)
{
    int tmp;

    buttonPressed = 0;

    if (waitForTrigger) {
        while (!PB_Get(0)) {}
        MXC_Delay(MXC_DELAY_MSEC(250));
    }

    // Debounce the button press.
    for (tmp = 0; tmp < 0x80000; tmp++) { __NOP(); }

    // Wait for serial transactions to complete.
#if USE_CONSOLE

    while (MXC_UART_ReadyForSleep(MXC_UART_GET_UART(CONSOLE_UART)) != E_NO_ERROR) {}

#endif // USE_CONSOLE
}
#endif // USE_BUTTON

int main(void)
{
#if USE_CONSOLE
    printf("****Low Power Mode Example****\n\n");
#endif // USE_CONSOLE

#if USE_ALARM
#if USE_CONSOLE
    printf("This code cycles through the MAX32690 power modes, using the RTC alarm \nto exit from "
           "each mode.  The modes will change every %d seconds.\n\n",
           DELAY_IN_SEC);
#endif // USE_CONSOLE
    MXC_NVIC_SetVector(RTC_IRQn, alarmHandler);
#endif // USE_ALARM

#if USE_BUTTON
#if USE_CONSOLE
    printf("This code cycles through the MAX32690 power modes. Use push button (SW2)\nto exit from "
           "each power mode and enter the next.\n\n");
#endif // USE_CONSOLE
    PB_RegisterCallback(0, buttonHandler);
    PB_IntEnable(0);
#endif // USE_BUTTON

#if USE_CONSOLE
    printf("Running in ACTIVE mode.\n");
#else
    MXC_SYS_ClockDisable(MXC_SYS_PERIPH_CLOCK_UART0);
#endif // USE_CONSOLE
    setTrigger(1);

#if USE_BUTTON
    MXC_LP_EnableGPIOWakeup((mxc_gpio_cfg_t *)&pb_pin[0]);
#endif // USE_BUTTON
#if USE_ALARM
    MXC_LP_EnableRTCAlarmWakeup();
#endif // USE_ALARM

    while (1) {
#if DO_SLEEP
#if USE_CONSOLE
        printf("Entering SLEEP mode.\n");
#endif // USE_CONSOLE
        setTrigger(0);
        MXC_LP_EnterSleepMode();
        printf("Waking up from SLEEP mode.\n");
#endif // DO_SLEEP

#if DO_LPM
#if USE_CONSOLE
        printf("Entering LPM mode.\n");
#endif // USE_CONSOLE
        setTrigger(0);
        MXC_LP_EnterLowPowerMode();
        printf("Waking up from LPM mode.\n");
#endif // DO_LPM

#if DO_UPM
#if USE_CONSOLE
        printf("Entering UPM mode.\n");
#endif // USE_CONSOLE
        setTrigger(0);
        MXC_LP_EnterMicroPowerMode();
        printf("Waking up from UPM mode.\n");
#endif // DO_UPM

#if DO_BACKUP
#if USE_CONSOLE
        printf("Entering BACKUP mode.\n");
#endif // USE_CONSOLE
        setTrigger(0);
        MXC_LP_EnterBackupMode();
#endif // DO_BACKUP

#if DO_STANDBY
#if USE_CONSOLE
        printf("Entering STANDBY mode.\n");
#endif // USE_CONSOLE
        setTrigger(0);
        MXC_LP_EnterStandbyMode();
#endif // DO_STANDBY
    }
}
