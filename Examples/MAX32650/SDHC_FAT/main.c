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
 * @brief   read and write sdhc
 * @details This example uses the sdhc and ffat to read/write the file system on
 *          an SD card. The Fat library used supports long filenames (see ffconf.h)
 *          the max length is 256 characters.
 *
 *          You must connect an sd card to the sd card slot.
 */

/***** Includes *****/
#include <stdio.h>

#include "board.h"
#include "cli.h"
#include "icc.h"
#include "mxc_errors.h"
#include "sdhc_example.h"
#include "uart.h"
#include "user-cli.h"

/***** Definitions *****/

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

/***** Functions *****/
int main(void)
{
    int err;

    printf("\n\n***** " TOSTRING(TARGET) " SDHC FAT Filesystem Example *****\n");

    MXC_ICC_Disable();

    if (sdhc_init() != E_NO_ERROR) {
        printf("Unable to initialize the SDHC card.\n");
        return E_BAD_STATE;
    }

    printf("SDHC Ready!\n");
    while (MXC_UART_GetActive(MXC_UART_GET_UART(CONSOLE_UART))) {}

    // Initialize CLI
    if ((err = MXC_CLI_Init(MXC_UART_GET_UART(CONSOLE_UART), user_commands, num_user_commands)) !=
        E_NO_ERROR) {
        return err;
    }

    // Run CLI
    while (1) {}

    return 0;
}
