/*
 * Copyright (C) 2018 Kees Bakker
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_sff
 * @{
 *
 * @file
 * @brief       Board common implementations for the SODAQ SFF boards
 *
 * @author      Kees Bakker <kees@sodaq.com>
 * @}
 */

#include "cpu.h"
#include "board.h"
#include "periph/gpio.h"

void board_init(void)
{
    /* set GPS off by default */
    GPS_ENABLE_OFF;
    gpio_init(GPS_ENABLE_PIN, GPIO_OUT);

    LED_RED_OFF;
    gpio_init(LED_RED_PIN, GPIO_OUT);

    LED_BLUE_OFF;
    gpio_init(LED_BLUE_PIN, GPIO_OUT);

    LED_GREEN_OFF;
    gpio_init(LED_GREEN_PIN, GPIO_OUT);

    /* initialize the CPU */
    cpu_init();
}
