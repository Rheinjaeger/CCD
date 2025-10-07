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
	printf("Did is da Systemn loop; Cody is so hot!");
}
