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
 * \file openlab-main.c
 *         Configuration for HiKoB OpenLab Fox
 *
 * \author
 *         Antoine Fraboulet <antoine.fraboulet.at.hikob.com>
 *         
 */

#include <string.h>

#include "platform.h"
#include "drivers/unique_id.h"
#define NO_DEBUG_HEADER
#define LOG_LEVEL LOG_LEVEL_INFO
#include "printf.h"


#include "contiki.h"
#include "contiki-net.h"

#define PROCESS_CONF_NO_PROCESS_NAMES 0

#if RIMEADDR_SIZE != 8
#error "RIME address size should be set to 8"
#endif /*RIMEADDR_SIZE == 8*/

/*---------------------------------------------------------------------------*/
void set_rime_addr()
{
    if (rimeaddr_node_addr.u8[0] != 0xBA)
    {
        /* Company 3 Bytes */
	rimeaddr_node_addr.u8[0] = 0xBA;
	rimeaddr_node_addr.u8[1] = 0xDB;
	rimeaddr_node_addr.u8[2] = 0x0B;
        /* Product Type 1 Byte */
	rimeaddr_node_addr.u8[3] = PLATFORM_TYPE;
        /* Product Version 1 Byte */
	rimeaddr_node_addr.u8[4] = PLATFORM_VERSION;
        /* Serial Number 3 Bytes */
	rimeaddr_node_addr.u8[5] = uid->uid8[9];
	rimeaddr_node_addr.u8[6] = uid->uid8[10];
	rimeaddr_node_addr.u8[7] = uid->uid8[11];
    }
}
/*---------------------------------------------------------------------------*/
static void
print_processes(struct process * const processes[])
{
#if !PROCESS_CONF_NO_PROCESS_NAMES
    printf(" Starting");
    while(*processes != NULL) 
    {
	printf(" '%s'", (*processes)->name);
	processes++;
    }
#endif /* !PROCESS_CONF_NO_PROCESS_NAMES */
    putchar('\n');
}
/*---------------------------------------------------------------------------*/
static void char_rx(handler_arg_t arg, uint8_t c)
{
    serial_line_input_byte(c);
}
/*---------------------------------------------------------------------------*/
int main()
{
    static uint32_t idle_count = 0;

    /* 
     * OpenLab Platform init
     * 
     */

    platform_init();
    // OpenLab default serial speed is 500kBps
    // uart_enable(uart_print, 115200);


    /*
     * Contiki core 
     *
     */

    clock_init();
    process_init();
    process_start(&etimer_process, NULL);
    ctimer_init();

    /* 
     * Network 
     *
     */

    netstack_init();
    set_rime_addr();

#if UIP_CONF_IPV6
    memcpy(&uip_lladdr.addr, &rimeaddr_node_addr, sizeof(uip_lladdr.addr));
    process_start(&tcpip_process, NULL);

    #if VIZTOOL_CONF_ON
    process_start(&viztool_process, NULL);
    #endif

    #if (!UIP_CONF_IPV6_RPL)
    {
	uip_ipaddr_t ipaddr;

	uip_ip6addr(&ipaddr, 0x2001, 0x630, 0x301, 0x6453, 0, 0, 0, 0);
	uip_ds6_set_addr_iid(&ipaddr, &uip_lladdr);
	uip_ds6_addr_add(&ipaddr, 0, ADDR_TENTATIVE);
    }
    #endif /* UIP_CONF_IPV6_RPL */
#endif /* UIP_CONF_IPV6 */

    /*
     * Serial communication
     *
     */

#if SLIP_ARCH_CONF_ENABLE
    // enable slip on CDC_ACM
    //slip_arch_init(0);
#else
    serial_line_init();
    uart_set_rx_handler(uart_print, char_rx, NULL);
#endif
    
    /*
     * Start
     *
     */

    print_processes(autostart_processes);
    autostart_start(autostart_processes);
    watchdog_start();

    while(1) 
    {
	int r;
	do 
	{
	    watchdog_periodic();
	    r = process_run();
	} while(r > 0);
	idle_count++;
    }

    return 0;
}
/*---------------------------------------------------------------------------*/
