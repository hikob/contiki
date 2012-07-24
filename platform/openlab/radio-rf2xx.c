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
 * \file radio-rf2xx.c
 *         Configuration for HiKoB OpenLab platforms
 *         This file contains wrappers around the OpenLab phy layer
 *
 * \author
 *         Antoine Fraboulet <antoine.fraboulet.at.hikob.com>
 *         
 */

#include <string.h>

#include "platform.h"
#define NO_DEBUG_HEADER
#define LOG_LEVEL LOG_LEVEL_INFO
#include "printf.h"
#include "event.h"
#include "phy.h"

#include "contiki.h"
#include "contiki-net.h"

/*---------------------------------------------------------------------------*/

#define RF2XX_CHANNEL   15
#define RF2XX_TX_POWER  PHY_POWER_0dBm 

static phy_packet_t     tx_pkt;
static void             tx_done(phy_status_t status);
static volatile int     tx_pkt_pending;

static phy_packet_t     rx_pkt;
static void             rx_done(phy_status_t status);
static volatile int     rx_pkt_pending;
static volatile int     rx_pkt_receiving;

static int              rf2xx_listening;

PROCESS(rf2xx_process, "rf2xx driver");

static int rf2xx_wr_on(void);
static int rf2xx_wr_off(void);

/*---------------------------------------------------------------------------*/

static int 
rf2xx_wr_init(void)
{
    log_debug("rf2xx_wr_init");

    tx_pkt_pending   = 0;
    rx_pkt_pending   = 0;
    rx_pkt_receiving = 0;

    rf2xx_listening  = 0;

    phy_reset(phy);
    phy_set_channel(phy, RF2XX_CHANNEL);
    process_start(&rf2xx_process, NULL);

    return 1;
}
  
/*---------------------------------------------------------------------------*/

/** Prepare the radio with a packet to be sent. */
static int 
rf2xx_wr_prepare(const void *payload, unsigned short payload_len)
{
    log_debug("rf2xx_wr_prepare %d :: %d",payload_len,soft_timer_time());

    tx_pkt.data   = tx_pkt.raw_data;
    tx_pkt.length = payload_len;
    memcpy(tx_pkt.data, payload, tx_pkt.length);

    return 1;
}

/*---------------------------------------------------------------------------*/

/** Send the packet that has previously been prepared. */
static int 
rf2xx_wr_transmit(unsigned short transmit_len)
{
    log_debug("rf2xx_wr_transmit %d :: %d", transmit_len, soft_timer_time());

    if (tx_pkt_pending == 1)
    {
	log_error("    Tx too early, another tx still active");
	return 0;
    }

    if (tx_pkt.length != transmit_len)
    {
	log_error("    pkt.length (%d) != transmit_len (%d) !", tx_pkt.length, transmit_len);
    }

// TODO:  if(packetbuf_attr(PACKETBUF_ATTR_RADIO_TXPOWER) > 0) {

    phy_idle(phy);

    tx_pkt_pending = 1;
    phy_tx_now(phy, &tx_pkt, tx_done);
    
    while (tx_pkt_pending)
    {
	clock_delay_usec(100);
    }

    if (rf2xx_listening)
    {
	rf2xx_wr_on();
    }

    return 1;
}

/*---------------------------------------------------------------------------*/

/** Prepare & transmit a packet. */
static int 
rf2xx_wr_send(const void *payload, unsigned short payload_len)
{
    log_debug("rf2xx_wr_send %d :: %d", payload_len,soft_timer_time());
    rf2xx_wr_prepare(payload, payload_len);
    rf2xx_wr_transmit(payload_len);
    return 1;
}

/*---------------------------------------------------------------------------*/

/** Read a received packet into a buffer. */
static int 
rf2xx_wr_read(void *buf, unsigned short buf_len)
{
    int len;

    log_debug("rf2xx_wr_read %d :: %d", buf_len, soft_timer_time());

    len = rx_pkt.length;
    if (buf_len < len)
    {
	len = buf_len;
    }

    memcpy(packetbuf_dataptr(), rx_pkt.data, len);
    return len;
}

/*---------------------------------------------------------------------------*/

/** Perform a Clear-Channel Assessment (CCA) to find out if there is
    a packet in the air or not. */
static int 
rf2xx_wr_channel_clear(void)
{
    log_debug("rf2xx_wr_channel_clear :: %d",soft_timer_time());

    // radio must be in idle before a cca for phy_cca(phy)
    // TODO: implement a different method that handles radio in Rx state.
    phy_idle(phy);

    return phy_cca(phy);
}

/*---------------------------------------------------------------------------*/

/** Check if the radio driver is currently receiving a packet */
static int 
rf2xx_wr_receiving_packet(void)
{
    log_debug("rf2xx_wr_receiving_packet : %d :: %d",rx_pkt_receiving,soft_timer_time());
    return rx_pkt_receiving;
}

/*---------------------------------------------------------------------------*/

/** Check if the radio driver has just received a packet */
static int
rf2xx_wr_pending_packet(void)
{
    log_debug("rf2xx_wr_pending_packet : %d :: %d",rx_pkt_pending,soft_timer_time());
    return rx_pkt_pending;
}

/*---------------------------------------------------------------------------*/

/** Turn the radio on. */
static int 
rf2xx_wr_on(void)
{
    log_debug("rf2xx_wr_on :: %d",soft_timer_time());

    // Enter RX now
    rx_pkt.data     = rx_pkt.raw_data;
    rx_pkt.length   = 0;
    rf2xx_listening = 1;
    // rx_time and timeout must stay at 0
    phy_rx_now(phy, &rx_pkt, rx_done);

    return 1;
}

/*---------------------------------------------------------------------------*/

/** Turn the radio <off. */
static int
rf2xx_wr_off(void)
{
    log_debug("rf2xx_wr_off :: %d",soft_timer_time());

    rf2xx_dig2_disable(phy);
    rf2xx_listening = 0;
    phy_idle(phy);

    return 1;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

const struct radio_driver rf2xx_driver =
  {
    .init             = rf2xx_wr_init,
    .prepare          = rf2xx_wr_prepare,
    .transmit         = rf2xx_wr_transmit,
    .send             = rf2xx_wr_send,
    .read             = rf2xx_wr_read,
    .channel_clear    = rf2xx_wr_channel_clear,
    .receiving_packet = rf2xx_wr_receiving_packet,
    .pending_packet   = rf2xx_wr_pending_packet,
    .on               = rf2xx_wr_on,
    .off              = rf2xx_wr_off,
 };

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

static void tx_done(phy_status_t status)
{
    tx_pkt_pending = 0;
    if (status == PHY_SUCCESS)
    {
        log_debug("Frame sent at %u, length: %u", tx_pkt.timestamp, tx_pkt.length);
    }
    else
    {
        log_debug("Error while sending %x", status);
    }
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

static void rx_done(phy_status_t status)
{
    // Check status
    switch (status)
    {
        case PHY_RX_TIMEOUT_ERROR:
            log_debug("PHY RX timeout\n");
            break;
        case PHY_RX_CRC_ERROR:
        case PHY_RX_ERROR:
        case PHY_RX_LENGTH_ERROR:
            log_debug("PHY RX error %x\n", status);
            break;
        case PHY_SUCCESS:
	    rx_pkt_pending = 1;
            log_debug("PHY Rx ok");
            break;
        default:
            // Weird status
            break;
    }
  process_poll(&rf2xx_process);
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/




event_status_t event_post_from_isr(event_queue_t queue, handler_t event, handler_arg_t arg)
{
    //log_debug("event post called %x %x",event,arg);
    event(arg);

/*
    if ((void*)event == (void*)tx_done)
    {
	log_debug("tx_done called");
	HALT();
    }

    if (rf2xx_event_list[rf2xx_event_wptr].event != NULL)
    {
	log_error("rf2xx_event != NULL, overwriting pending event");
    }

    rf2xx_event_list[rf2xx_event_wptr].event = event;
    rf2xx_event_list[rf2xx_event_wptr].arg = arg;
    log_debug("event_post %d :: %d",rf2xx_event_wptr,soft_timer_time());

    rf2xx_event_wptr = rf2xx_event_wptr % RF2XX_EVENT_SIZE;
    rf2xx_event_size = rf2xx_event_size + 1;
    
    process_poll(&rf2xx_process);
*/

    return EVENT_OK;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/


PROCESS_THREAD(rf2xx_process, ev, data)
{
    int len;

    PROCESS_BEGIN();

    while(1) 
    {
	PROCESS_YIELD_UNTIL(ev == PROCESS_EVENT_POLL);

	if (rx_pkt_pending)
	{
	    rx_pkt_pending = 0;
	    packetbuf_clear();
	    
	    if (rx_pkt.length > 0)
	    {
		/*
		log_debug("Packet Received !!");
		log_debug("\tlength = %u", rx_pkt.length);
		log_debug("\ttimestamp = %u", rx_pkt.timestamp);
		log_debug("\trssi = %d", rx_pkt.rssi);
		log_debug("\tlqi = %d", rx_pkt.lqi);
		*/
		
		len = rf2xx_wr_read(packetbuf_dataptr(), PACKETBUF_SIZE);
		packetbuf_set_datalen(len);
		NETSTACK_RDC.input();
	    }
	}
    }

    PROCESS_END();
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
