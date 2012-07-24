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
 * Copyright (C) 2011,2012 HiKoB.
 */

/**
 * \file button-sensor.c
 *         Configuration for HiKoB OpenLab Fox
 *
 * \author
 *         Antoine Fraboulet <antoine.fraboulet.at.hikob.com>
 *         
 */

#include "platform.h"
#define LOG_LEVEL LOG_LEVEL_INFO
#include "printf.h"

#include "contiki.h"
#include "lib/sensors.h"

#define SENSOR_NAME "Button"

const struct sensors_sensor button_sensor;

static volatile uint8_t button_active = 0;

/*---------------------------------------------------------------------------*/
static int 
value(int type)
{
    (void)type;
    return button_state();
}
/*---------------------------------------------------------------------------*/
static void 
dev_button_handler(void *dummy)
{
    ENERGEST_ON(ENERGEST_TYPE_IRQ);
    sensors_changed(&button_sensor); 
    ENERGEST_OFF(ENERGEST_TYPE_IRQ);
}
/*---------------------------------------------------------------------------*/
static int
configure(int type, int c)
{
    switch (type)
    {
    case SENSORS_HW_INIT:
	return 1;
    case SENSORS_ACTIVE:
	if (c == 0) 
	{
	    button_set_handler(NULL, NULL);
	    button_active = 0;
	} 
	else 
	{
	    button_set_handler(dev_button_handler, NULL);
	    button_active = 1;
	}
	return 1;
    default:
	log_error("button: unknown command");
	break;
    }
    return 0;
}
/*---------------------------------------------------------------------------*/
static int
status(int type)
{
    int ret = 0;
    switch (type) 
    {
    case SENSORS_ACTIVE:
    case SENSORS_READY:
	ret = button_active;
	break;
    default:
	break;
    }
    return ret;
}
/*---------------------------------------------------------------------------*/
SENSORS_SENSOR(button_sensor, SENSOR_NAME, value, configure, status);

