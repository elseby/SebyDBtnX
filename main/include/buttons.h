/*
 * buttons.h
 *
 *  Created on: 5 mag 2025
 *      Author: sebmar
 */
#pragma once
 
 
#include "config.h"
#include "esp_err.h"
#include <stdint.h>

#define STORAGE_NAMESPACE "storage"

#define SETTINGS_PER_BUTTON 10
 
/*
#BUTTON_PRESS_DOWN
#BUTTON_PRESS_UP
#BUTTON_PRESS_REPEAT
#BUTTON_PRESS_REPEAT_DONE
BUTTON_SINGLE_CLICK
BUTTON_DOUBLE_CLICK
#BUTTON_MULTIPLE_CLICK
BUTTON_LONG_PRESS_START
BUTTON_LONG_PRESS_HOLD
BUTTON_LONG_PRESS_UP
#BUTTON_PRESS_REPEAT_DONE
#BUTTON_PRESS_END
*/ 
 
typedef enum: uint8_t {
    BTN_EVT_NONE=0,
    BTN_EVT_SINGLE_CLICK=5,
    BTN_EVT_DOUBLE_CLICK=10,
    BTN_EVT_LONG_PRESS_START=15,
    BTN_EVT_LONG_PRESS_HOLD=20,
    BTN_EVT_LONG_PRESS_UP=25,
    BTN_EVT_PRESS_DOWN=30,
    BTN_EVT_PRESS_UP=35
} button_event_type_t;


typedef struct {
	uint8_t buttonId;
	button_event_type_t eventType;
} button_event_data_t;


typedef struct {
	button_event_type_t eventType;
	uint8_t command[6];
	uint8_t where[2];
	uint8_t interface;
	uint8_t status;
} button_settings_command_t;
 

typedef struct {
	button_settings_command_t buttons[NUM_OF_BUTTONS][SETTINGS_PER_BUTTON];
	uint8_t ledProfile[NUM_OF_BUTTONS];
} dbtx_settings_t;


 extern dbtx_settings_t settings;

 void initButtons();
 esp_err_t saveButtonsSettings(void);
 void stateChangedTrigger();
 
 
 