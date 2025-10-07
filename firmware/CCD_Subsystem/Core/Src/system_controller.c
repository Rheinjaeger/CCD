/*
 * system_controller.c
 *
 *  Created on: Sep 27, 2025
 *      Author: Javier
 */


#include "system_controller.h"
#include "w5500.h"
#include "stm32l0xx_hal.h"
#include <string.h>
#include <stdio.h>

extern uint8_t Rx_Buffer[2048];

void System_Init(SystemController *ctrl) {
    // Init Ethernet
    ctrl->capture = placeholder_capture;
    ctrl->transmit = placeholder_transmit;
    ctrl->reset_system = placeholder_reset;
    ctrl->set_idle = placeholder_set_idle;
}

void placeholder_capture(void) {
	printf("Placeholder: Capture()\n");
}

void placeholder_transmit(void) {
	printf("Placeholder: Capture()\n");
}

void placeholder_reset(void) {
	printf("Placeholder: Capture()\n");
}

void placeholder_set_idle(void) {
	printf("Placeholder: Capture()\n");
}

void System_Loop(void) {
    const char *msg = "ping";
    Write_SOCK_Data_Buffer(0, (uint8_t *)msg, strlen(msg));

    HAL_Delay(1000);

    if (Read_W5500_SOCK_Byte(0, Sn_IR) & IR_RECV) {
        Write_W5500_SOCK_Byte(0, Sn_IR, IR_RECV);
        uint16_t size = Read_SOCK_Data_Buffer(0, Rx_Buffer);
        Rx_Buffer[size] = '\0';

        if (strcmp((char *)Rx_Buffer, "pong") == 0) {
            HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
        }
    }
}
