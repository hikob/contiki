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
 * \file openlab-port.c
 *         Contiki add-ons for HiKoB OpenLab Newt
 *
 * \author
 *         Antoine Fraboulet <antoine.fraboulet.at.hikob.com>
 *         
 */

#include <stdint.h>

#include "printf.h"

#include "contiki.h"


/*-----------------------------------------------------------------------------------*/
/*
 * Function wrappers for Contiki
 *
 */
int 
puts(const char *str)
{
    printf((char*)str);
    return 0;
}

void 
abort(void)
{
    log_error("abort() called");
    HALT();
}

/*-----------------------------------------------------------------------------------*/
/*
 * Empty wrappers for Openlab event library
 */
void event_debug() { }
void event_init() { }
/*-----------------------------------------------------------------------------------*/
/*
 * Empty wrappers for advanced options of Openlab soft_timer library 
 */
void soft_timer_debug() { }
void soft_timer_update(handler_arg_t arg, uint16_t count) { }
/*-----------------------------------------------------------------------------------*/
