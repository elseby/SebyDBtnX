# See the build system documentation in IDF programming guide
# for more information about component CMakeLists.txt files.

idf_component_register(
    SRCS
    	"buttons.c" 
    	"commands.c"
		"decoder.c"
    	"busrtx.c"
    	"httpserver.c"
    	"leds.c"
    	"main.c"
    	"otaupload.c"
    	"setupcommand.c"
    	"utils.c"
    	
    INCLUDE_DIRS "include"
    PRIV_INCLUDE_DIRS
    REQUIRES 
    	nvs_flash 
    	esp_http_server 
    	esp_wifi 
    	esp_eth 
    	esp_driver_rmt 
    	esp_driver_uart
    	led_strip 
    	esp_driver_gpio 
    	bootloader_support 
    	esp_timer
    	app_update
     PRIV_REQUIRES
	EMBED_TXTFILES 
		web/config.html    
		web/otaupload.html
)
