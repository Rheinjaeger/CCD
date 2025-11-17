/*
 * system_controller.c
 *
 *  Created on: Sep 27, 2025
 *      Author: Javier
 */


#include "system_controller.h"
#include "w5500_port.h"
#include "socket.h"
#include "stm32l0xx_hal.h"
#include <string.h>
#include <stdio.h>

#define UDP_PORT 6000
#define BUF_SIZE 2048

uint8_t buf[BUF_SIZE];

// --- Network Configuration ---
wiz_NetInfo gWIZNETINFO = {
    .mac = {0x00, 0x08, 0xDC, 0x11, 0x22, 0x33},
    .ip =  {192, 168, 56, 2},
    .sn =  {255, 255, 255, 0},
    .gw =  {192, 168, 56, 1},
    .dns = {8, 8, 8, 8},
    .dhcp = NETINFO_STATIC
};

// --- W5500 Initialization ---
void W5500_LowLevelInit(void)
{
    // Register SPI and CS callbacks
    reg_wizchip_cs_cbfunc(W5500_Select, W5500_Deselect);
    reg_wizchip_spi_cbfunc(W5500_ReadByte, W5500_WriteByte);
    reg_wizchip_spiburst_cbfunc(W5500_ReadBurst, W5500_WriteBurst);

    // Initialize buffer sizes
    uint8_t memsize[2][8] = {{2,2,2,2,2,2,2,2}, {2,2,2,2,2,2,2,2}};
    wizchip_init(memsize[0], memsize[1]);

    // Apply static network configuration
    wizchip_setnetinfo(&gWIZNETINFO);
}

// --- UDP Echo Server ---
void UDP_Echo_Server(void)
{
    SOCKET s = 0;
    int32_t len;
    uint8_t sender_ip[4];
    uint16_t sender_port;

    len = getSn_RX_RSR(s);
    if (len > 0)
    {
        len = recvfrom(s, buf, len, sender_ip, &sender_port);
        if (len > 0)
        {
            printf("Received %ld bytes from %d.%d.%d.%d:%d\n", len,
                   sender_ip[0], sender_ip[1], sender_ip[2], sender_ip[3], sender_port);

            sendto(s, (uint8_t*)"pong", 4, sender_ip, sender_port);
        }
    }
}

void System_Init(SystemController *ctrl)
{
    // Initialize W5500 hardware interface
    W5500_LowLevelInit();  // configure SPI callbacks and PHY reset

    // Configure W5500 network parameters
    wiz_NetInfo netInfo = {
        .mac  = {0x00, 0x08, 0xDC, 0x12, 0x34, 0x56},
        .ip   = {192, 168, 56, 2},   // your board IP
        .sn   = {255, 255, 255, 0},  // subnet mask
        .gw   = {192, 168, 56, 1},   // gateway (your PC)
        .dns  = {8, 8, 8, 8},
        .dhcp = NETINFO_STATIC
    };
    ctlnetwork(CN_SET_NETINFO, (void*)&netInfo);

    // Verify chip communication
    uint8_t version = getVERSIONR();
    if (version != 0x04) {
        printf("W5500 not detected! VERSIONR=0x%02X\r\n", version);
    } else {
        printf("W5500 OK! VERSIONR=0x%02X\r\n", version);
    }

    // Open UDP socket for echo testing
    uint8_t socket_num = 0;
    uint16_t port = 6000;
    if (socket(socket_num, Sn_MR_UDP, port, 0) != socket_num) {
        printf("Failed to open UDP socket\r\n");
    } else {
        printf("UDP socket %d open on port %d\r\n", socket_num, port);
    }
}





