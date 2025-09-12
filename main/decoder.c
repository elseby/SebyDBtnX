/*
 * decoder.c
 *
 *  Created on: 31 mar 2025
 *      Author: sebmar
 */

#include "config.h"
#include "esp_log.h"
#include "busrtx.h"
#include "include/busrtx.h"
#include "httpserver.h"
#include "include/buttons.h"
#include <stdint.h>
#include <string.h>
#include <strings.h>

static const char *TAG = "decoder";

void decodeLen7Message(uint8_t idch, uint8_t *rxMsgData);

/************************************************************************************
 *	RXDATA DECODER
 *************************************************************************************/
 
 void btox1(char *xp, uint8_t *bb, int n) {
	const char xx[] = "0123456789ABCDEF";
	for(int i=0;i<n;i++){
		xp[i*3] = xx[bb[i]>>4];
		xp[(i*3)+1] = xx[bb[i]&0xf];
		xp[(i*3)+2] = ' ';
	}
}
 
void sendHibusMsgToWs(hibus_rx_frame_t *rxMsg, char prefix) {
	if(isWsConnected()){
		size_t slen = (rxMsg->len*3)+2;
		char wsMsgTxt[slen+1];
		wsMsgTxt[0] = prefix;
		btox1(wsMsgTxt+1, rxMsg->data, rxMsg->len);
		wsMsgTxt[slen-1]=0;
		// invio msg
		sendStringToWs(wsMsgTxt);
	}
}
 
hibus_rx_frame_t lastRxMsg={0};

void rxDataDecoder(hibus_rx_frame_t *rxMsg) {
	// scarto l'A5 ack
	if (rxMsg->len == 1 && rxMsg->data[0] == 0xA5) {
		goto rxDecoderExit;
	}
	
	// ignoro stesso messaggio
	if(lastRxMsg.len==rxMsg->len && memcmp(lastRxMsg.data, rxMsg->data, rxMsg->len)==0){
		// stesso messaggio dell'ultimo elaborato
		goto rxDecoderExit;
	}
	
	// copio il messaggio nell'ultimo
	memcpy(&lastRxMsg,&rxMsg, sizeof(hibus_rx_frame_t));

	button_settings_command_t *btn;
	for (int idbtn = 0; idbtn < NUM_OF_BUTTONS; idbtn++) {
		for(int i=0;i<SETTINGS_PER_BUTTON;i++){
			btn = &settings.buttons[idbtn][i];
			if(btn->eventType!=BTN_EVT_NONE){ 
				// decodifica messaggi di stato interfaccia locale len 7
				if(
					rxMsg->len == 7 
					&& rxMsg->data[1]==0xb8 
					&& rxMsg->data[3]==0x12
					&& !btn->interface
					&& rxMsg->data[2]==btn->where[0]
					){
						btn->status = rxMsg->data[4];
						if(i==0){
							stateChangedTrigger();
						}
				}else if(
					// interfaccia altro livello
					//stato esteso
					//<A8 EC FF 01 F0 B8 62 12 00 2A A3 	//livello 1
					//<A8 EC 00 FF 0F B8 68 12 01 DF A3		//montante principale 
					rxMsg->len == 11
					&& rxMsg->data[1] == 0xec
					&& (btn->interface==0xff ? 0xff : btn->interface)==rxMsg->data[3]
					&& rxMsg->data[5]==0xb8
					&& rxMsg->data[6]==btn->where[0] 
					&& rxMsg->data[7]==0x12
					){
						btn->status = rxMsg->data[8];
						if(i==0){
							stateChangedTrigger();
						}
				}
			}	
		}
	}


//	} else if (rxMsg.len == 11) {
//		// msg da len=11
//		if(rxMsg.data[1] == 0xec){
//			// interfaccia altro livello
//			//stato esteso
//			//<A8 EC FF 01 F0 B8 62 12 00 2A A3 
//			for (int idbtn = 0; idbtn < NUM_OF_BUTTONS; idbtn++) {
//				button_settings_command_t *btn;
//				for(int i=0;i<SETTINGS_PER_BUTTON;i++){
//					btn = &buttons[idbtn][i];
//					if(btn->interface==rxMsg.data[3]){
//						decodeLen7Message(idbtn, rxMsg.data+4);
//					}
//				}
//			}
//
//		}else if(rxMsg.data[1] == 0xd1 
//			&& rxMsg.data[3] == 0x00 
//			&& rxMsg.data[4] == 0x00
//		){
//			// termo
//			//19.5
//			//I (672898) Chan: l:11->a8 d1 0 3 2 c2 c 12 27 2b a3
//						
//		}
//	}
	
	rxDecoderExit:
	
	// send to websocket (if is connected)
	sendHibusMsgToWs(rxMsg,'<');
}

void decodeLen7Message(uint8_t idbtn, uint8_t *msgData) {
	if(msgData[1]!=0xb8 || msgData[3]!=0x12){
		// only state frame...
		return;
	}
	
	button_settings_command_t *btn;
	for(int i=0;i<SETTINGS_PER_BUTTON;i++){
		btn = &settings.buttons[idbtn][i];
		
		// ignore not configured button, other interface or command message
		if( btn->eventType!=BTN_EVT_NONE 
			&& !btn->interface
			&& msgData[2]==btn->where[0]
		){  
			
			ESP_LOGI(TAG, "btnid:%x status:%x rxnewstatue:%x",idbtn, btn->status, msgData[4]);
			
			// same Where
			if(btn->status != msgData[4]){
	//			ESP_LOGI(TAG, "btnid:%x %x %x",idbtn, btn.status, rxMsg.data[4]);
				btn->status = msgData[4];
				if(i==0){
					stateChangedTrigger();
				}
			}
		
		}		
	}
	
}











