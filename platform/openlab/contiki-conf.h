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
 *         Configuration for HiKoB OpenLab platforms
 *
 * \author
 *         Antoine Fraboulet <antoine.fraboulet.at.hikob.com>
 *         
 */

#ifndef __OPENLAB_CONTIKI_CONF_H__
#define __OPENLAB_CONTIKI_CONF_H__

#include <stdint.h>

/* ---------------------------------------- */
/*
 *  Clock module and rtimer support
 *
 */

#define CLOCK_CONF_SECOND 100

typedef unsigned int   clock_time_t;
typedef unsigned short rtimer_clock_t;

/* ---------------------------------------- */
/*
 * Cortex M3 / ARM
 *
 */

typedef uint8_t  u8_t;
typedef uint16_t u16_t;
typedef uint32_t u32_t;
typedef int8_t   s8_t;
typedef int16_t  s16_t;
typedef int32_t  s32_t;

#define CC_BYTE_ALIGNED __attribute__ ((packed, aligned(1)))
/* Prefix for relocation sections in ELF files */
#define REL_SECT_PREFIX ".rel"

/* ---------------------------------------- */
/*
 * Networking
 *
 */

#define WITH_UIP                        1

typedef unsigned int uip_stats_t;

#ifndef NETSTACK_CONF_MAC
//#define NETSTACK_CONF_MAC           nullmac_driver
#define NETSTACK_CONF_MAC           csma_driver
#endif /* NETSTACK_CONF_MAC */

#ifndef NETSTACK_CONF_RDC
//#define NETSTACK_CONF_RDC           nullrdc_driver
//#define NETSTACK_CONF_RDC           cxmac_driver
//#define NETSTACK_CONF_RDC           contikimac_driver
#define NETSTACK_CONF_RDC           sicslowmac_driver
#endif /* NETSTACK_CONF_RDC */

#ifndef NETSTACK_CONF_RDC_CHANNEL_CHECK_RATE
#define NETSTACK_CONF_RDC_CHANNEL_CHECK_RATE 8
#endif /* NETSTACK_CONF_RDC_CHANNEL_CHECK_RATE */

#ifndef NETSTACK_CONF_RADIO
#define NETSTACK_CONF_RADIO         rf2xx_driver
#endif /* NETSTACK_CONF_RADIO */

#ifndef NETSTACK_CONF_FRAMER
//#define NETSTACK_CONF_FRAMER        framer_nullmac
#define NETSTACK_CONF_FRAMER        framer_802154
#endif /* NETSTACK_CONF_FRAMER */

#if UIP_CONF_IPV6
#define RIMEADDR_CONF_SIZE          8
#define UIP_CONF_ICMP6              1
#define UIP_CONF_UDP                1
#define UIP_CONF_TCP                1
#define UIP_CONF_IPV6_RPL           1
#define NETSTACK_CONF_NETWORK       sicslowpan_driver
#define SICSLOWPAN_CONF_COMPRESSION SICSLOWPAN_COMPRESSION_HC06
#else  /* UIP_CONF_IPV6 */
#define RIMEADDR_CONF_SIZE          8
#define NETSTACK_CONF_NETWORK       rime_driver
#endif /* UIP_CONF_IPV6 */

/* ---------------------------------------- */
/*
 * Contiki internals
 *
 */

#define WITH_ASCII                      1

#define CCIF
#define CLIF

/* include the project config */
/* PROJECT_CONF_H might be defined in the project Makefile */
#ifdef PROJECT_CONF_H
#include PROJECT_CONF_H
#endif /* PROJECT_CONF_H */

#endif /* __OPENLAB_CONTIKI_CONF_H__ */
