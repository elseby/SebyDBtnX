/*
 * leds.h
 *
 *  Created on: 5 mag 2025
 *      Author: sebmar
 */

#pragma once

#include <stdint.h>
#include "config.h"
#include "soc/gpio_num.h"

typedef struct {
	uint32_t red;
	uint32_t green;
	uint32_t blue;
} led_color_t;

//typedef enum: uint8_t {
//    LED_PROFILE_OFF=0,
//    LED_PROFILE_DEFAULT
//} led_profile_type_t;

typedef enum: uint8_t {
    LED_STATE_UNSET=0,
    LED_STATE_INACTIVE,
    LED_STATE_ACTIVE,
    LED_STATE_UPDOWN,
    LED_STATE_HTTPENABLING,
    LED_STATE_HTTPENABLE,
} led_state_type_t;

typedef struct {
	led_color_t unset;
	led_color_t inactive;
	led_color_t active;
	led_color_t upDown;
	led_color_t httpEnabling;
	led_color_t httpEnable;
} led_profile_colors_t;

extern led_color_t ledColors[NUM_OF_LEDS];

//static const led_color_t ledHttpEnableColor = {.red=1,.green=0,.blue=0};
//static const led_color_t ledHttpEnablingColor = {.red=2,.green=2,.blue=3};
//static const led_color_t ledUnsetColor = {.red=0,.green=0,.blue=0};
//static const led_color_t ledOffColor = {.red=1,.green=1,.blue=2};
//static const led_color_t ledOnColor = {.red=2,.green=1,.blue=0};

void setAllLed(led_state_type_t ledState);
void updateLedColors(void);
void ledsInitSequence();

void setLedColor(uint8_t idled, led_state_type_t ledState);

static const gpio_num_t led_strip_gpio = GPIO_LEDSTRIP;

void ledsInit();


