/*
 * net_if.c
 *
 *  Created on: Nov 16, 2025
 *      Author: Shrek
 */

#include "net_if.h"
#include "w5500_port.h"
#include "socket.h"
#include <stdio.h>
#include <string.h>

static void wizchip_spi_attach(void) {
    reg_wizchip_cs_cbfunc(W5500_Select, W5500_Deselect);
    reg_wizchip_spi_cbfunc(W5500_ReadByte, W5500_WriteByte);
    reg_wizchip_spiburst_cbfunc(W5500_ReadBurst, W5500_WriteBurst);
}

void NET_Init(void) {
    uint8_t memsize[2][8] = {{2,2,2,2,2,2,2,2},{2,2,2,2,2,2,2,2}};
    uint8_t link;

    W5500_HardReset();
    wizchip_spi_attach();

    // Init internal TX/RX buffers per socket
    if (wizchip_init(memsize[0], memsize[1]) != 0) {
        // handle error if desired
    }

    // Your static network info (edit to your LAN)
    wiz_NetInfo net = {
        .mac = {0x00,0x08,0xDC,0x12,0x34,0x56},
        .ip  = {192,168,10,2},
        .sn  = {255,255,255,0},
        .gw  = {192,168,10,1},
        .dns = {8,8,8,8},
        .dhcp = NETINFO_STATIC
    };

    wizchip_setnetinfo(&net);

    // Wait for PHY link
    do {
        ctlwizchip(CW_GET_PHYLINK, (void*)&link);
    } while (link == PHY_LINK_OFF);
    // At this point, ping should reply.
}

