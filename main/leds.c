/*
 * leds.c
 *
 *  Created on: 5 mag 2025
 *      Author: sebmar
 */

#include "include/leds.h"
#include "buttons.h"
#include "led_strip.h"
#include "led_strip_rmt.h"
#include "led_strip_types.h"
#include <stdint.h>

static led_strip_handle_t led_strip = NULL;

led_color_t ledColors[NUM_OF_LEDS];
//led_profile_type_t ledProfile = LED_PROFILE_DEFAULT;

/******************************************************************
*	LED PROFILES
*******************************************************************/

#define LED_PROFILES_COUNT 5

static const led_profile_colors_t ledProfileOff = {
	.unset={.red=0,.green=0,.blue=0},
	.inactive={.red=0,.green=0,.blue=0},
	.active={.red=0,.green=0,.blue=0},
	.upDown={.red=0,.green=0,.blue=0},
	.httpEnabling={.red=2,.green=2,.blue=3},
	.httpEnable={.red=1,.green=0,.blue=0},
};

static const led_profile_colors_t ledProfileDefault = {
	.unset={.red=0,.green=0,.blue=0},
	.inactive={.red=0,.green=1,.blue=0},
	.active={.red=3,.green=1,.blue=0},
	.upDown={.red=1,.green=3,.blue=3},
	.httpEnabling={.red=2,.green=2,.blue=3},
	.httpEnable={.red=1,.green=0,.blue=0},
};

static const led_profile_colors_t ledProfileWhite = {
	.unset={.red=0,.green=0,.blue=0},
	.inactive={.red=1,.green=1,.blue=2},
	.active={.red=2,.green=1,.blue=0},
	.upDown={.red=2,.green=1,.blue=0},
	.httpEnabling={.red=2,.green=2,.blue=3},
	.httpEnable={.red=1,.green=0,.blue=0},
};

static const led_profile_colors_t ledProfileBluePurple = {
	.unset={.red=0,.green=0,.blue=0},
	.inactive={.red=0,.green=0,.blue=1},
	.active={.red=2,.green=0,.blue=1},
	.upDown={.red=1,.green=2,.blue=0},
	.httpEnabling={.red=2,.green=2,.blue=3},
	.httpEnable={.red=1,.green=0,.blue=0},
};

static const led_profile_colors_t ledProfileBlueOrange = {
	.unset={.red=0,.green=0,.blue=0},
	.inactive={.red=0,.green=0,.blue=1},
	.active={.red=3,.green=1,.blue=0},
	.upDown={.red=0,.green=2,.blue=1},
	.httpEnabling={.red=2,.green=2,.blue=3},
	.httpEnable={.red=1,.green=0,.blue=0},
};

static const led_profile_colors_t* ledProfiles[LED_PROFILES_COUNT] = {
	&ledProfileDefault,
	&ledProfileOff,	
	&ledProfileWhite,
	&ledProfileBluePurple,
	&ledProfileBlueOrange,
};

//***************************************************************

void ledsInit(){
	for(uint8_t i=0;i<NUM_OF_LEDS;i++) {
		ledColors[i] = ledProfileDefault.unset;// ledOffColor;
	}
	// LED strip initialization with the GPIO and pixels number
	led_strip_config_t strip_config = {
		.led_model = LED_MODEL_SK6812,
		.color_component_format = LED_STRIP_COLOR_COMPONENT_FMT_GRB,
		.strip_gpio_num =led_strip_gpio,
		.max_leds = NUM_OF_LEDS,
	};
	led_strip_rmt_config_t rmt_config = {
		.mem_block_symbols = 64,
		.resolution_hz = 10000000, // 10MHz
		.flags.with_dma = false,
	};
	ESP_ERROR_CHECK(
		led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip));		
}

void setAllLed(led_state_type_t ledState){
	// default led colors	
	for(uint8_t i=0;i<NUM_OF_LEDS;i++){
		setLedColor(i, ledState);
	}
	updateLedColors();	
}

void updateLedColors(void){
	for (size_t i = 0; i < NUM_OF_LEDS; i++) {
		led_strip_set_pixel(led_strip, i, ledColors[i].red, ledColors[i].green, ledColors[i].blue);
	}
	led_strip_refresh(led_strip);
}

void setLedColor(uint8_t idled, led_state_type_t ledState){
	uint8_t ledProfile = settings.ledProfile[idled]; 
	if(ledProfile>LED_PROFILES_COUNT){
		ledProfile=0;
	}
	ledColors[idled] = ledState == LED_STATE_INACTIVE ? ledProfiles[ledProfile]->inactive:
		ledState == LED_STATE_ACTIVE ? ledProfiles[ledProfile]->active :
		ledState == LED_STATE_UPDOWN ? ledProfiles[ledProfile]->upDown :
		ledState == LED_STATE_HTTPENABLE ? ledProfiles[ledProfile]->httpEnable :
		ledState == LED_STATE_HTTPENABLING ? ledProfiles[ledProfile]->httpEnabling :
		ledProfiles[ledProfile]->unset;
}

void ledsInitSequence(){
	for(int i=0;i<=NUM_OF_LEDS;i++){
		if(i>0){
			ledColors[i-1].red = 0;
			ledColors[i-1].green = 0;
			ledColors[i-1].blue = 0;
		}
		if(i<NUM_OF_LEDS){
			ledColors[i].red = 30;
			ledColors[i].green = 30;
			ledColors[i].blue = 30;
		}
		updateLedColors();
		vTaskDelay(80 / portTICK_PERIOD_MS);
	}	
}

