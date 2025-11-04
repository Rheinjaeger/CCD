/*
 * system_controller.h
 *
 *  Created on: Sep 27, 2025
 *      Author: Javier
 */

#ifndef INC_SYSTEM_CONTROLLER_H_
#define INC_SYSTEM_CONTROLLER_H_

#include "stm32l0xx_hal.h"
//#include "wizchip_conf.h"
//#include "socket.h"

// --- Data Structures ---
typedef struct {
    void (*capture)(void);
    void (*transmit)(void);
    void (*reset_system)(void);
    void (*set_idle)(void);
} SystemController;

// --- Function Prototypes ---
void System_Init(SystemController *ctrl);
void W5500_LowLevelInit(void);
void UDP_Echo_Server(void);

#endif /* INC_SYSTEM_CONTROLLER_H_ */
