/*
 * command_handler.h
 *
 *  Created on: Sep 22, 2025
 *      Author: Javier
 */

#ifndef INC_COMMAND_HANDLER_H_
#define INC_COMMAND_HANDLER_H_

#include "system_controller.h"

typedef struct {
	void (*parse_command)(const char *cmd);
	void (*execute_command)(const char *cmd);
}CommandHandler;

void init_command_handler(CommandHandler *handler, SystemController *controller);

#endif /* INC_COMMAND_HANDLER_H_ */
