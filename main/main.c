#include "driver/gpio.h"
#include "include/buttons.h"
#include "include/leds.h"
#include "include/busrtx.h"
#include "include/httpserver.h"
#include "freertos/idf_additions.h"
#include "nvs_flash.h"
#include <stdbool.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include "include/decoder.h"

void nvsInit(void);

void app_main(void) {

	nvsInit();
	
	ledsInit();

	initButtons();
	
	initApWebServer();	

	hibus_config_t busConfig = {
		.gpio_rx_port = GPIO_HIBUS_RX_PIN,
		.gpio_tx_port = GPIO_HIBUS_TX_PIN,
		.rx_handler = &rxDataDecoder
	};

	hibusInit(&busConfig);
	
	ledsInitSequence();

	printf("SebyD Ready!\n");

	setAllLed(LED_STATE_INACTIVE);
	
	#ifdef WIFIONATSTARTUP
	wifiToggle();
	#endif
	
	
	
	vTaskDelete(NULL);

}

void nvsInit(void){
	// Initialize NVS
	esp_err_t ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
		ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK(ret);
}
