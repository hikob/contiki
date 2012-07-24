/*
 * This file is part of HiKoB Openlab.
 *
 * HiKoB Openlab is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation, version 3.
 *
 * HiKoB Openlab is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with HiKoB Openlab. If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * Copyright (C) 2012 HiKoB.
 */

/**
 * \file
 *         leds-arch.c for OpenLab Fox platform
 * \author
 *         Clement Burin Des Roziers <clement.burin-des-roziers.at.hikob.com> 
 *         Antoine Fraboulet <antoine.fraboulet.at.hikob.com>
 */

#include "platform.h"
#include "platform/agile-fox/agilefox.h"

#include "leds.h"

/*
 * platform_leds_setup() is a copy/paste from 
 * openlab/platform/agile-fox/agilefox_leds.c
 *
 */

#define LED_PORT   gpioB
#define LED_0_PIN  GPIO_PIN_10
#define LED_1_PIN  GPIO_PIN_12

void platform_leds_setup(void)
{
    // Configure the LEDs
    // Enable port B
    gpio_enable(LED_PORT);

    // Set output pins B10 and B12
    gpio_set_output(LED_PORT, LED_0_PIN);
    gpio_set_output(LED_PORT, LED_1_PIN);

    // Clear LEDs
    leds_arch_set(0);
}

/*
 * Contiki support 
 *
 */

void leds_arch_init(void)
{
    /* already done by platform_init() */
}

unsigned char leds_arch_get(void)
{
    int l0 = gpio_pin_read(LED_PORT, LED_0_PIN);
    int l1 = gpio_pin_read(LED_PORT, LED_1_PIN);

    return (l0 ? LED_0 : 0) | (l1 ? LED_1 : 0);
}

void leds_arch_set(unsigned char leds)
{
    if (leds & LED_0)
    {
        gpio_pin_clear(LED_PORT, LED_0_PIN);
    }
    else
    {
        gpio_pin_set(LED_PORT, LED_0_PIN);
    }

    if (leds & LED_1)
    {
        gpio_pin_clear(LED_PORT, LED_1_PIN);
    }
    else
    {
        gpio_pin_set(LED_PORT, LED_1_PIN);
    }
}
