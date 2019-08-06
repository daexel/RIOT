/*
 * Copyright (C) 2018 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_sodaq-sff
 * @{
 *
 * @file
 * @brief       Board specific definitions for the SODAQ SFF boards
 *
 * @author      Kees Bakker <kees@sodaq.com>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"
#include "periph_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    xtimer configuration
 * @{
 */
#define XTIMER_WIDTH        (16)
/** @} */

/**
 * @name    LED pin definitions and handlers
 * @{
 */
#define LED0_PIN            GPIO_PIN(PA, 16)

#define LED0_PORT           PORT->Group[PA]
#define LED0_MASK           (1 << 16)

#define LED0_OFF            (LED0_PORT.OUTCLR.reg = LED0_MASK)
#define LED0_ON             (LED0_PORT.OUTSET.reg = LED0_MASK)
#define LED0_TOGGLE         (LED0_PORT.OUTTGL.reg = LED0_MASK)

#define LED_GREEN_PIN         LED0_PIN
#define LED_GREEN_OFF         LED0_OFF
#define LED_GREEN_ON          LED0_ON
#define LED_GREEN_TOGGLE      LED0_TOGGLE

#define LED1_PIN            GPIO_PIN(PA, 14)

#define LED1_PORT           PORT->Group[PA]
#define LED1_MASK           (1 << 14)

#define LED1_OFF            (LED1_PORT.OUTSET.reg = LED1_MASK)
#define LED1_ON             (LED1_PORT.OUTCLR.reg = LED1_MASK)
#define LED1_TOGGLE         (LED1_PORT.OUTTGL.reg = LED1_MASK)

#define LED_RED_PIN         LED1_PIN
#define LED_RED_OFF         LED1_OFF
#define LED_RED_ON          LED1_ON
#define LED_RED_TOGGLE      LED1_TOGGLE

#define LED2_PIN            GPIO_PIN(PA, 15)

#define LED2_PORT           PORT->Group[PA]
#define LED2_MASK           (1 << 15)

#define LED2_OFF            (LED2_PORT.OUTSET.reg = LED2_MASK)
#define LED2_ON             (LED2_PORT.OUTCLR.reg = LED2_MASK)
#define LED2_TOGGLE         (LED2_PORT.OUTTGL.reg = LED2_MASK)

#define LED_BLUE_PIN         LED2_PIN
#define LED_BLUE_OFF         LED2_OFF
#define LED_BLUE_ON          LED2_ON
#define LED_BLUE_TOGGLE      LED2_TOGGLE
/** @} */

/**
 * @name    GPS Time Pulse
 * @{
 */
#define GPS_TIMEPULSE_PIN   GPIO_PIN(PA, 17)
#define GPS_TIMEPULSE_MODE  GPIO_IN
/** @} */

/**
 * @name    GPS Enable
 * @{
 */
#define GPS_ENABLE_PIN      GPIO_PIN(PA, 18)

#define GPS_ENABLE_PORT     PORT->Group[PA]
#define GPS_ENABLE_MASK     (1 << 18)

#define GPS_ENABLE_ON       (GPS_ENABLE_PORT.OUTSET.reg = GPS_ENABLE_MASK)
#define GPS_ENABLE_OFF      (GPS_ENABLE_PORT.OUTCLR.reg = GPS_ENABLE_MASK)
/** @} */

/**
 * @brief Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
