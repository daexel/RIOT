/*
 * Copyright (C) 2019 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
/**
 * @ingroup     drivers_dcf77
 * @{
 *
 * @file
 * @brief       Device driver implementation for the dcf 77
 *              longwave time signal and standard-frequency radio station
 *
 * @author      Michel Gerlach <michel.gerlach@haw-hamburg.de>
 *
 * @}
 */

#include <stdint.h>
#include <string.h>

#include "log.h"
#include "assert.h"
#include "xtimer.h"
#include "timex.h"
#include "periph/gpio.h"

#include "dcf77.h"
#include "dcf77_internal.h"
#include "dcf77_params.h"

#define ENABLE_DEBUG    (1)
#include "debug.h"

/* Persistent level longer than 1200ms starts a new cycle  */
#define DCF77_PULSE_START_HIGH_THRESHOLD        (1200000U)  /*~1000ms*/
/* Every pulse send by the DCF longer than 130ms is interpreted as 1 */
#define DCF77_PULSE_WIDTH_THRESHOLD             (140000U)   /*~130ms*/
/* If an expected pulse is not detected within 2,5s, something is wrong */
#define DCF77_TIMEOUT                           (2500000U)  /*~2500ms*/
/* Number of bits in a cycle*/
#define DCF77_READING_CYCLE                     (59)

#define DCF77_MINUTE_MASK                       (0xFE00000ULL)
#define DCF77_HOUR_MASK                         (0x7E0000000ULL)
#define DCF77_DATE_MASK                         (0x1FFFFFC00000000ULL)

#define DCF77_MINUTE_SHIFT                      (21)
#define DCF77_HOUR_SHIFT                        (29)
#define DCF77_DATE_SHIFT                        (36)

static void _level_cb_high(dcf77_t *dev)
{
    switch (dev->internal_state) {
        case DCF77_STATE_START:
            DEBUG("[dcf77] EVENT START 1 !\n");
            dev->stopTime = xtimer_now_usec();
            if ((dev->stopTime - dev->startTime) >
                DCF77_PULSE_START_HIGH_THRESHOLD) {
                memset(&dev->bitseq.bits, 0, sizeof dev->bitseq.bits);
                dev->internal_state = DCF77_STATE_RX;
            }else{
                dev->internal_state = DCF77_STATE_IDLE;
            }
            break;
        case DCF77_STATE_RX:
            DEBUG("[dcf77] EVENT RX 1 !\n");
            dev->startTime = xtimer_now_usec();
            break;
    }
}

static void _level_cb_low(dcf77_t *dev)
{
    switch (dev->internal_state) {
        case DCF77_STATE_IDLE:
            DEBUG("[dcf77] EVENT IDLE 0  !\n");
            dev->startTime = xtimer_now_usec();
            dev->internal_state = DCF77_STATE_START;
            break;
        case DCF77_STATE_RX:
            DEBUG("[dcf77] EVENT RX 0 !\n");
            dev->stopTime = xtimer_now_usec();
            if ((dev->stopTime - dev->startTime) >
                DCF77_PULSE_WIDTH_THRESHOLD) {
                dev->bitseq.bits |=  1ULL << dev->bitCounter;
            }
            dev->bitCounter++;
            if (dev->bitCounter >= DCF77_READING_CYCLE) {
                dev->bitCounter = 0;
                dev->startTime = xtimer_now_usec();
                dev->last_bitseq.bits = dev->bitseq.bits;
                dev->internal_state = DCF77_STATE_START;
            }
            break;
    }
}

static void _level_cb(void *arg)
{
    dcf77_t *dev = (dcf77_t *)arg;

    if (gpio_read(dev->params.pin)) {
        _level_cb_high(dev);
    }
    else {
        _level_cb_low(dev);
    }
}

/**
 * @brief   Initialize the Device
 *
 * @param   dev     device
 * @param   params  device_params
 *
 * @retval  DCF77_OK             Success
 * @retval  DCF77_INIT_ERROR     Error in initialization
 */

int dcf77_init(dcf77_t *dev, const dcf77_params_t *params)
{
    DEBUG("dcf77_init\n");

    /* check parameters and configuration */
    assert(dev && params);
    dev->params = *params;
    dev->internal_state = DCF77_STATE_IDLE;
    dev->bitCounter = 0;
    if (!gpio_init_int(dev->params.pin, dev->params.in_mode, GPIO_BOTH,
                       _level_cb,
                       dev)) {
        return DCF77_OK;
    }
    else {
        gpio_irq_disable(dev->params.pin);
        return DCF77_INIT_ERROR;
    }
}

/**
 * @brief   Formats the information after all bits of a cycle have been received
 *
 * @param   dev               device
 * @param   time              datastruct for timeinformation
 *
 * @retval  DCF77_OK          Success
 * @retval  DCF77_NOCSUM      Parity Bits aren't correct
 */

int dcf77_get_time(dcf77_t *dev, struct tm *time)
{
    assert(dev);

    if (dev->last_bitseq.val.mesz == 2) {
        time->tm_isdst = 1;
    }
    else {
        time->tm_isdst = 0;
    }

    uint8_t minute = 10 * dev->last_bitseq.val.minute_h +
                     dev->last_bitseq.val.minute_l;
    if (__builtin_parity((dev->last_bitseq.bits >> DCF77_MINUTE_SHIFT) &
                         (DCF77_MINUTE_MASK >> DCF77_MINUTE_SHIFT)) !=
        dev->last_bitseq.val.minute_par) {
        return DCF77_NOCSUM;
    }

    uint8_t hour = 10 * dev->last_bitseq.val.hour_h +
                   dev->last_bitseq.val.hour_l;
    if (__builtin_parity((dev->last_bitseq.bits >> DCF77_HOUR_SHIFT) &
                         (DCF77_HOUR_MASK >> DCF77_HOUR_SHIFT)) !=
        dev->last_bitseq.val.hour_par) {
        return DCF77_NOCSUM;
    }

    uint8_t mday = 10 * dev->last_bitseq.val.day_h + dev->last_bitseq.val.day_l;

    uint8_t wday =  dev->last_bitseq.val.wday;

    uint8_t month = 10 * dev->last_bitseq.val.month_h +
                    dev->last_bitseq.val.month_l;

    uint8_t year = 10 * dev->last_bitseq.val.year_h +
                   dev->last_bitseq.val.year_l;
    if (__builtin_parity((dev->last_bitseq.bits >> DCF77_DATE_SHIFT) &
                         (DCF77_DATE_MASK >> DCF77_DATE_SHIFT)) !=
        dev->last_bitseq.val.date_par) {
        return DCF77_NOCSUM;
    }

    time->tm_min = minute;
    time->tm_hour = hour;
    time->tm_mday = mday;
    time->tm_wday = wday;
    time->tm_mon = month;
    time->tm_year = 100 + year;

    return DCF77_OK;
}
