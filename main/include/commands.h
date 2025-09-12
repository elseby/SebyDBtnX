/*
 * commands.h
 *
 *  Created on: 6 mag 2025
 *      Author: sebmar
 */
#pragma once

#include "busrtx.h"
#include "buttons.h"
#include <stdbool.h>

typedef struct{
	const hibus_tx_msg_mode_t* msgMode;
	bool valid;
}command_return_t;

void processCommand(button_settings_command_t *bs, uint8_t *dataToSend, int *dataLen, command_return_t* ret);



