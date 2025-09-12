/*
 * commands.c
 *
 *  Created on: 6 mag 2025
 *      Author: sebmar
 */

#include "commands.h"
#include "busrtx.h"
#include "buttons.h"
#include <stdbool.h>
#include <stdint.h>

static const uint8_t basicCommands[] = {
	0x00,0x01,0xff,		//switch (toggle 0xff)
	0x0a,0x08,0x09,		//cover
	0x1d,0x2d,0x3d,0x4d,0x5d,0x6d,0x7d,0x8d,0x9d,0x03,0x04,	//dimmer
	0x16,0x26,0x36,0x46,0x56,0x66,0x76,0x86,
	0x0b,0x1b,0x2b,0x3b,0x4b,0x5b,0x6b,0x7b,0x8b,0x9b,
};

bool isBasicCommand(uint8_t cmd){
	for(int i=0;i<sizeof(basicCommands);i++){
		if(basicCommands[i]==cmd){
			return true;
		}
	}
	return false;
}

void putData(uint8_t data, uint8_t *dataToSend, int *dataLen){
	if(dataToSend!=NULL){
		dataToSend[(*dataLen)++]=data;
	}
}

void processCommand(button_settings_command_t *bs, uint8_t *dataToSend, int *dataLen, command_return_t* ret){
	
	if(bs->eventType==0){
		ret->valid = false;
	}
	
	uint8_t cmd0 = bs->command[0]; 
	
	ret->valid = true;
	
	if(isBasicCommand(cmd0)){
		// controllo se toggle switch
		if(cmd0==0xff) {
			cmd0 = bs->status==0x01 ? 0x00 : 0x01;
		}
		//compongo il where
		if( (bs->interface>0 && bs->interface<=9) || bs->interface==0xff ){
			putData(bs->interface==0xff ? 0xe4 : 0xec ,dataToSend, dataLen);
			putData(0x00,dataToSend, dataLen);
			putData(0x00,dataToSend, dataLen);
			putData(0x00,dataToSend, dataLen);
			ret->msgMode = TX_MODE_COMMAND_X3;
		}
		putData(bs->where[0],dataToSend, dataLen);
		putData(bs->where[1],dataToSend, dataLen);
		putData(0x12,dataToSend, dataLen);
		putData(cmd0,dataToSend, dataLen);
	}else if(cmd0==0xcd){
		// CEN 
//		#comando
//		CD [where 01-99] 14 [button 0-99]
		putData(0xcd,dataToSend, dataLen);
		putData(bs->where[0],dataToSend, dataLen);
		putData(0x14,dataToSend, dataLen);
		putData(bs->command[1],dataToSend, dataLen);
	}else{
		ret->valid = false;	
	}

}
