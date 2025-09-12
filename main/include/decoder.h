/*
 * buttons.h
 *
 *  Created on: 5 mag 2025
 *      Author: sebmar
 */
#pragma once
 
#include "busrtx.h"


void rxDataDecoder(hibus_rx_frame_t *rxFrame);
void sendHibusMsgToWs(hibus_rx_frame_t *rxMsg, char prefix);

