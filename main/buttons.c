/*
 * button.c
 *
 *  Created on: 5 mag 2025
 *      Author: sebmar
 */

#include "include/buttons.h"
#include "busrtx.h"
#include "config.h"
#include "include/httpserver.h"
#include "include/commands.h"
#include "include/leds.h"
#include "button_gpio.h"
#include "esp_attr.h"
#include "esp_log.h"
#include "freertos/idf_additions.h"
#include "include/config.h"
#include "iot_button.h"
#include "nvs.h"
#include "soc/gpio_num.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <sys/param.h>
#include "driver/gpio.h"

static const char *TAG = "Btn";
static const char * BUTTONS_NVS_KEY = "buttons";

static const  gpio_num_t buttonsGpios[] = {
#ifdef DBTN1
	GPIO_BUTTON_1   
	
#elifdef DBTN4
	GPIO_BUTTON_1,
	GPIO_BUTTON_2,
	GPIO_BUTTON_3,
	GPIO_BUTTON_4
	
#elifdef DBTN6
	GPIO_BUTTON_1,
	GPIO_BUTTON_2,
	GPIO_BUTTON_3,
	GPIO_BUTTON_4,
	GPIO_BUTTON_5,
	GPIO_BUTTON_6
#elifdef DBTN8
	GPIO_BUTTON_1,
	GPIO_BUTTON_2,
	GPIO_BUTTON_3,
	GPIO_BUTTON_4,
	GPIO_BUTTON_5,
	GPIO_BUTTON_6,
	GPIO_BUTTON_7,
	GPIO_BUTTON_8
#endif
};

esp_err_t loadButtonsSettings(void);
void sendButtonCommand(button_settings_command_t *buttonSetting);
void changeStateTask(void *arg);

dbtx_settings_t settings;

QueueHandle_t xQueueButton = NULL;
QueueHandle_t xQueueChangeState= NULL;

IRAM_ATTR
static void button_event_cb(void *arg, void *user_data) {
	int userData = (int)user_data;
	button_event_data_t eventData = {
		.buttonId = (userData>>8) & 0xff,
		.eventType = userData & 0xff
	};
	xQueueSendFromISR(xQueueButton, &eventData, 0);
}

bool isButtonFirstAndLastPressed(){
	return !gpio_get_level(buttonsGpios[0]) && !gpio_get_level(buttonsGpios[NUM_OF_BUTTONS-1]);
}

void buttonTask(void *arg) {
	button_event_data_t eventData;
	button_settings_command_t *buttonSettings;
	for(;;){
		if (xQueueReceive(xQueueButton, &eventData,(TickType_t)portMAX_DELAY)) {
			
			// controllo se long press dei tasti 1-4
			if(
				((eventData.buttonId==0 && eventData.eventType==BTN_EVT_LONG_PRESS_START)||(eventData.buttonId==(NUM_OF_BUTTONS-1) && eventData.eventType==BTN_EVT_LONG_PRESS_START))
				&& isButtonFirstAndLastPressed()
				){
				vTaskDelay(2500 / portTICK_PERIOD_MS);
				if(isButtonFirstAndLastPressed()){
					setAllLed(LED_STATE_HTTPENABLING);
					wifiToggle();
					// attendo il release dei button
					while(isButtonFirstAndLastPressed()){
						vTaskDelay(250 / portTICK_PERIOD_MS);
						xQueueReset(xQueueButton);
					}
					vTaskDelay(250 / portTICK_PERIOD_MS);						
					xQueueReset(xQueueButton);
					continue;
				}
				xQueueReset(xQueueButton);
				continue;
			}

			ESP_LOGI(TAG, "ButtonCommand! ev:%i btn:%x",eventData.eventType,eventData.buttonId);
			
			// prelevo i comandi da verificare
			buttonSettings=settings.buttons[eventData.buttonId];
			for(int s=0;s<SETTINGS_PER_BUTTON;s++){
				// cerco quali corrispondono all'evento
				if(buttonSettings[s].eventType==eventData.eventType){
					sendButtonCommand(&buttonSettings[s]);
				}
			}
		}
	}
}

//**************************************************************************************/

void initButtons(){
	//init buttons variable

	ESP_ERROR_CHECK(loadButtonsSettings());
	
	xQueueButton = xQueueCreate(20, sizeof(button_event_data_t));
	xQueueChangeState= xQueueCreate(2, 0);

	int userData;
	
	for(int b=0;b<NUM_OF_BUTTONS;b++){
		
		settings.buttons[b][0].status=0xff;
		
		button_gpio_config_t gpio_btn_cfg = {
			.gpio_num = buttonsGpios[b],
			.active_level = 0,
			.enable_power_save = false,
			.disable_pull = 0};
		button_config_t btn_cfg = {
			.long_press_time = CONFIG_BUTTON_LONG_PRESS_TIME_MS,
			.short_press_time = CONFIG_BUTTON_SHORT_PRESS_TIME_MS,
		};
		button_handle_t gpio_btn;
		iot_button_new_gpio_device(&btn_cfg, &gpio_btn_cfg, &gpio_btn);

		// button id byteH, event byteL 
		userData = ((b<<8) & 0xff00) | BTN_EVT_SINGLE_CLICK;
		iot_button_register_cb( gpio_btn, BUTTON_SINGLE_CLICK, NULL, button_event_cb,  (void*)userData );

		userData = ((b<<8) & 0xff00) | BTN_EVT_DOUBLE_CLICK;
		iot_button_register_cb( gpio_btn, BUTTON_DOUBLE_CLICK, NULL, button_event_cb,  (void*)userData );

		userData = ((b<<8) & 0xff00) | BTN_EVT_LONG_PRESS_START;
		iot_button_register_cb( gpio_btn, BUTTON_LONG_PRESS_START, NULL, button_event_cb,  (void*)userData );

		userData = ((b<<8) & 0xff00) | BTN_EVT_LONG_PRESS_HOLD;
		iot_button_register_cb( gpio_btn, BUTTON_LONG_PRESS_HOLD, NULL, button_event_cb,  (void*)userData );

		userData = ((b<<8) & 0xff00) | BTN_EVT_LONG_PRESS_UP;
		iot_button_register_cb( gpio_btn, BUTTON_LONG_PRESS_UP, NULL, button_event_cb,  (void*)userData );

		userData = ((b<<8) & 0xff00) | BTN_EVT_PRESS_DOWN;
		iot_button_register_cb( gpio_btn, BUTTON_PRESS_DOWN, NULL, button_event_cb,  (void*)userData );

		userData = ((b<<8) & 0xff00) | BTN_EVT_PRESS_UP;
		iot_button_register_cb( gpio_btn, BUTTON_PRESS_UP, NULL, button_event_cb,  (void*)userData );
	}	
	
	xTaskCreate(buttonTask, "buttonTask", 1024*3, NULL, 0, NULL);
	xTaskCreate(changeStateTask, "changeStateTask", 2048, NULL, 0, NULL);
	
}

esp_err_t loadButtonsSettings(void){

    nvs_handle_t my_handle;
    esp_err_t err;

    // Open
    err = nvs_open(STORAGE_NAMESPACE, NVS_READONLY, &my_handle);
	if(err==ESP_ERR_NVS_NOT_FOUND || err==ESP_ERR_NVS_INVALID_LENGTH){
		memset(&settings, 0x0, sizeof(settings));
		return saveButtonsSettings();
	}else{
	    if (err != ESP_OK) return err;
	    size_t required_size = sizeof(settings);
		ESP_LOGI(TAG, "Reading buttons settings size %d",required_size);
	    err = nvs_get_blob(my_handle, BUTTONS_NVS_KEY, &settings, &required_size);
	    if(err==ESP_ERR_NVS_INVALID_LENGTH){
			nvs_close(my_handle);
			memset(&settings, 0x0, sizeof(settings));
			return saveButtonsSettings();
		}
	    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) return err;
	    // Close
	    nvs_close(my_handle);
	    return ESP_OK;	
	}
}

esp_err_t saveButtonsSettings(void){
	
    nvs_handle_t my_handle;
    esp_err_t err;

    // Open
    err = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &my_handle);
    if (err != ESP_OK) return err;

//    size_t required_size = sizeof(ChannelSetting) * CH_LEN;
//    err = nvs_get_blob(my_handle, CHANNEL_NVS_KEY, channels, &required_size);
//    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) return err;

	size_t required_size =  sizeof(settings);
	ESP_LOGI(TAG, "Writing buttons settings size %d",required_size);

    err = nvs_set_blob(my_handle, BUTTONS_NVS_KEY, &settings, required_size);

    if (err != ESP_OK) return err;

    // Commit
    err = nvs_commit(my_handle);
    if (err != ESP_OK) return err;

	ESP_LOGI(TAG, "Committed ");

    // Close
    nvs_close(my_handle);
    return ESP_OK;

}

void sendButtonCommand(button_settings_command_t *bs){
	int dataLen=0;
	uint8_t dataToSend[8];
	command_return_t ret;
	ret.msgMode = TX_MODE_COMMAND_X3_IF_NOT_ACK;
	processCommand(bs, dataToSend, &dataLen, &ret);
	if(ret.valid){
		hibusTxData(dataToSend, dataLen, ret.msgMode);
		
//		ESP_LOGI(TAG, "Data Sent! %x %x %x %x %x %x %x %x ",
//			dataToSend[0],
//			dataToSend[1],
//			dataToSend[2],
//			dataToSend[3],
//			dataToSend[4],
//			dataToSend[5],
//			dataToSend[6],
//			dataToSend[7]
//		);
	}
}




//**************************************************************************************/
void changeStateTask(void *arg) {
	button_settings_command_t *btn;
	for(;;){
		if (xQueueReceive(xQueueChangeState, NULL,(TickType_t)portMAX_DELAY)) {
			for(int idbtn=0;idbtn<NUM_OF_BUTTONS;idbtn++){
				btn = &settings.buttons[idbtn][0];	//take the first
				//I determine the state
				// if NOT off or stop(cover)
				setLedColor(
					idbtn, 
					btn->eventType==BTN_EVT_NONE ? LED_STATE_UNSET :
					(btn->status==0x01 || btn->status==0x0a || btn->status==0xff) ? LED_STATE_INACTIVE :
					(btn->status==0x08 || btn->status==0x09) ? LED_STATE_UPDOWN :
					LED_STATE_ACTIVE
				);
//				ledColors[idbtn] = btn.eventType==BTN_EVT_NONE ? ledUnsetColor :
//					(btn.status==0x01 || btn.status==0x0a || btn.status==0xff) ? ledOffColor :
//					ledOnColor;
			}
			updateLedColors();		
		}
	}
}

void stateChangedTrigger(){
	xQueueSend(xQueueChangeState, NULL, 0);
}

