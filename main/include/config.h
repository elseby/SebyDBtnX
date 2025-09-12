#pragma once
//************************************************************************************************/
// Select MODEL with number of buttons
//************************************************************************************************/
//#define DBTN1		// only for test (breadboard) 1 button
#define DBTN4		// SebyDBtn4 (2 module with 4 button)
//#define DBTN4_TEST	// SebyDBtn4 only for test 4 button and TP (test points)
//#define DBTN6		// SebyDBtn6 (3 module with 6 button)
//#define DBTN8		// SebyDBtn8 (4 module with 4 button)
//#define DBTN8_OLD		// SebyDBtn8 (only for test first HW version)
//************************************************************************************************/

#ifdef DBTN4_TEST	
	#define DBTN4
	#define WIFIONATSTARTUP
	#define TESTPOINT_A GPIO_NUM_45
	#define TESTPOINT_B GPIO_NUM_46
#endif

#define GPIO_LEDSTRIP  GPIO_NUM_38

#ifdef DBTN1
	#define NUM_OF_BUTTONS 1
	#define GPIO_BUTTON_1  GPIO_NUM_6
	#define GPIO_HIBUS_TX_PIN  GPIO_NUM_2
	#define GPIO_HIBUS_RX_PIN  GPIO_NUM_1
	#define HIBUS_INVERT_RX
#elifdef DBTN4
	#define NUM_OF_BUTTONS 4
	#define GPIO_BUTTON_1  GPIO_NUM_10
	#define GPIO_BUTTON_2  GPIO_NUM_11
	#define GPIO_BUTTON_3  GPIO_NUM_12
	#define GPIO_BUTTON_4  GPIO_NUM_13
	#define GPIO_HIBUS_TX_PIN  GPIO_NUM_35
	#define GPIO_HIBUS_RX_PIN  GPIO_NUM_34
	#define HIBUS_INVERT_RX
#elifdef DBTN6
	#define NUM_OF_BUTTONS 6
	#define GPIO_BUTTON_1  GPIO_NUM_10
	#define GPIO_BUTTON_2  GPIO_NUM_11
	#define GPIO_BUTTON_3  GPIO_NUM_12
	#define GPIO_BUTTON_4  GPIO_NUM_13
	#define GPIO_BUTTON_5  GPIO_NUM_14
	#define GPIO_BUTTON_6  GPIO_NUM_37
	#define GPIO_HIBUS_TX_PIN  GPIO_NUM_35
	#define GPIO_HIBUS_RX_PIN  GPIO_NUM_34
	#define HIBUS_INVERT_RX
#endif

#ifndef DBTN8_OLD
	#define HIBUS_INVERT_RX
#endif
#ifdef DBTN8_OLD
	#define DBTN8
#endif

#ifdef DBTN8
	#define NUM_OF_BUTTONS 8
	#define GPIO_BUTTON_1  GPIO_NUM_10
	#define GPIO_BUTTON_2  GPIO_NUM_11
	#define GPIO_BUTTON_3  GPIO_NUM_12
	#define GPIO_BUTTON_4  GPIO_NUM_13
	#define GPIO_BUTTON_5  GPIO_NUM_14
	#define GPIO_BUTTON_6  GPIO_NUM_37
	#define GPIO_BUTTON_7  GPIO_NUM_36
	#define GPIO_BUTTON_8  GPIO_NUM_33
	#define GPIO_HIBUS_TX_PIN  GPIO_NUM_35
	#define GPIO_HIBUS_RX_PIN  GPIO_NUM_34
#endif

#define NUM_OF_LEDS NUM_OF_BUTTONS





