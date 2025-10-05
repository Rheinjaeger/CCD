/*
 * system_controller.c
 *
 *  Created on: Sep 27, 2025
 *      Author: Shrek
 */


#include "system_controller.h"
#include "w5500.h"
#include "stm32l0xx_hal.h"
#include <string.h>
#include <stdio.h>

void System_Init(void) {
    // Init Ethernet
    init_chip();
    printf("System: W5500 Initialized.\n");
}

void System_Loop(void) {
    uint8_t buffer[128];

    // Wait for data and Echo
    uint16_t len = W5500_Receive(buffer, sizeof(buffer));
    if (len > 0) {
        printf("Received %d bytes: %s\n", len, buffer);
        W5500_Send(buffer, len); // echo
    }
}
