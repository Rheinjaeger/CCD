/*
 * system_controller.h
 *
 *  Created on: Sep 27, 2025
 *      Author: Javier
 */

#ifndef INC_SYSTEM_CONTROLLER_H_
#define INC_SYSTEM_CONTROLLER_H_

typedef struct {
	void (*capture)(void);
	void (*transmit)(void);
	void (*reset_system)(void);
	void (*set_idle)(void);
} SystemController;

void System_Init(SystemController *ctrl);
void System_Loop(void);

void placeholder_capture(void);
void placeholder_transmit(void);
void placeholder_reset(void);
void placeholder_set_idle(void);

#endif /* INC_SYSTEM_CONTROLLER_H_ */
