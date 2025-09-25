/*
 * command_handler.c
 *
 *  Created on: Sep 22, 2025
 *      Author: Javier
 */

#include "command_handler.h"
#include <string.h>
#include <stdio.h>

// Placeholder for actual controller implementation
static SystemController *sc;

static void parse_command_impl(const char *cmd) {
    printf("Parsing command: %s\n", cmd);
    // TODO: add command validation, structure, tokenizing
}

static void execute_command_impl(const char *cmd) {
    printf("Executing command: %s\n", cmd);

    if (strcmp(cmd, "CAPTURE") == 0) {
        sc->capture();
    } else if (strcmp(cmd, "TRANSMIT") == 0) {
        sc->transmit();
    } else if (strcmp(cmd, "RESET") == 0) {
        sc->reset_system();
    } else if (strcmp(cmd, "IDLE") == 0) {
        sc->set_idle();
    } else {
        printf("Unknown command: %s\n", cmd);
    }
}

void init_command_handler(CommandHandler *handler, SystemController *controller) {
    sc = controller;
    handler->parse_command = parse_command_impl;
    handler->execute_command = execute_command_impl;
}
