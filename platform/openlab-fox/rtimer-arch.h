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
 * \file
 *         rtimer-arch.h for OpenLab platforms
 * \author
 *         Clement Burin Des Roziers <clement.burin-des-roziers.at.hikob.com> 
 *         Antoine Fraboulet <antoine.fraboulet.at.hikob.com>
 */

#ifndef __RTIMER_ARCH_H__
#define __RTIMER_ARCH_H__

#include "platform.h"
#include "soft_timer_delay.h"

#include "contiki-conf.h"
#include "sys/rtimer.h"


/*
 * rtimer are mapped on real timer
 */

#define RTIMER_TIMER        tim3
#define RTIMER_CHANNEL      TIMER_CHANNEL_1

#define RTIMER_ARCH_SECOND (SOFT_TIMER_FREQUENCY)

/*
 * Contiki support functions
 */

void rtimer_arch_set(rtimer_clock_t t);

rtimer_clock_t rtimer_arch_now(void);

#endif /* __RTIMER_ARCH_H__ */
