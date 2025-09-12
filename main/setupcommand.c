/*
 * btsetupcommand.c
 *
 *  Created on: 19 mar 2025
 *      Author: sebmar
 */
#include "buttons.h"
#include "commands.h"
#include "config.h"
#include "esp_app_desc.h"
#include "include/utils.h"
#include "esp_http_server.h"
#include "esp_cpu.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/task.h"
#include <regex.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/param.h>

static const char *TAG = "BtCmd";

#define BUF_LEN 64

void commandResetCallback(char* msg, regmatch_t *matches,
						  regex_t *regex) {
	esp_cpu_reset(0);
}

void processSetupCommand(char *msg, char *retMsg){

	char buf[BUF_LEN];
	// cerco prima se c'e' un comando scmd
	//scmd: REBOOT
    if (httpd_query_key_value(msg, "scmd", buf, BUF_LEN) == ESP_OK){
        ESP_LOGI(TAG, "Found command => %s", buf);

        if(strcmp("REBOOT", buf)==0){
			//reboot
			esp_cpu_reset(0);
			return;
		}
        if(strcmp("RESET", buf)==0){
			memset(&settings, 0x0, sizeof(settings));
			saveButtonsSettings();
			strcpy(retMsg,"OK. Reset complete.");
			return;
		}
    }	

	uint8_t idBtn;
	button_settings_command_t btnCmd[SETTINGS_PER_BUTTON];
	memset(&btnCmd, 0x0, sizeof(button_settings_command_t)*SETTINGS_PER_BUTTON);
    if (httpd_query_key_value(msg, "btn", buf, BUF_LEN) == ESP_OK){
		idBtn = atoi(buf);
		ESP_LOGI(TAG, "BTNSET id:'%i'", idBtn);
		if(idBtn>=NUM_OF_BUTTONS){
			strcpy(retMsg,"Wrong ID Button 0-7");
			return;
		}
    }else{
		strcpy(retMsg,"No button selected!");
		return;
	}	

	char key[5];
	int idCmd=0;
	for(int idCmdEl=0;idCmdEl<SETTINGS_PER_BUTTON;idCmdEl++){
		sprintf(key, "e%i", idCmdEl);
		if (httpd_query_key_value(msg, key, buf, BUF_LEN) == ESP_OK){
			btnCmd[idCmd].eventType=atoi(buf);
			sprintf(key, "c0%i", idCmdEl);
			if (httpd_query_key_value(msg, key, buf, BUF_LEN) == ESP_OK){
				if(putCharHexToByteArray(btnCmd[idCmd].command, 1, buf)!=ESP_OK){
					strcpy(retMsg,"What error!");
					return;
				}
			}
			if(btnCmd[idCmd].command[0]==0xCD){
				sprintf(key, "c1%i", idCmdEl);
				if (httpd_query_key_value(msg, key, buf, BUF_LEN) == ESP_OK){
					if(putCharHexToByteArray(btnCmd[idCmd].command+1, 1, buf)!=ESP_OK){
						strcpy(retMsg,"What error!");
						return;
					}
				}
			}
			
			sprintf(key, "w%i", idCmdEl);
			if (httpd_query_key_value(msg, key, buf, BUF_LEN) == ESP_OK){
				if(putCharHexToByteArray(btnCmd[idCmd].where, 2, buf)!=ESP_OK){
					strcpy(retMsg,"Where error!");
					return;
				}
			}
			sprintf(key, "i%i", idCmdEl);
			if (httpd_query_key_value(msg, key, buf, BUF_LEN) == ESP_OK){
				if(putCharHexToByteArray(&btnCmd[idCmd].interface, 1, buf)!=ESP_OK){
					strcpy(retMsg,"Interface error!");
					return;
				}				
			}
			
			// validate command
			command_return_t ret;
			processCommand(&btnCmd[idCmd], NULL, NULL, &ret);
			if(!ret.valid){
				strcpy(retMsg,"Command invalid!");
				return;
			}
		}
		sprintf(key, "lp");
		if (httpd_query_key_value(msg, key, buf, BUF_LEN) == ESP_OK){
			if(putCharHexToByteArray(&settings.ledProfile[idBtn], 1, buf)!=ESP_OK){
				strcpy(retMsg,"Led profile error!");
				return;
			}
		}
		
		idCmd++;
	}
	memcpy(settings.buttons[idBtn], btnCmd, sizeof(btnCmd));

	//salvo i dati
	saveButtonsSettings();
	
	strcpy(retMsg,"OK. Button Saved.");
	
}


/********************************************************************
*
*	CHANNEL INFO
*
*********************************************************************/

esp_err_t sendChannelInfo(httpd_req_t *req){
	char buf[200];

	httpd_resp_set_type(req, "application/json");

 	const esp_app_desc_t* espAppDescr = esp_app_get_description();

	esp_err_t ret;
	sprintf(buf,"{\"app\":{\"ver\":\"%s\",\"date\":\"%s\",\"time\":\"%s\"},\"ledprofiles\":[\"%02x\""
		,espAppDescr->version
		,espAppDescr->date
		,espAppDescr->time
		,settings.ledProfile[0]
	);
	ret = httpd_resp_sendstr_chunk(req, buf);
	if(ret!=ESP_OK) return ret;

	for(int idBtn=1;idBtn<NUM_OF_BUTTONS;idBtn++){
		sprintf(buf,",\"%02x\""
			,settings.ledProfile[idBtn]
		);
		ret = httpd_resp_sendstr_chunk(req, buf);
		if(ret!=ESP_OK) return ret;
	}
	
	ret=httpd_resp_sendstr_chunk(req, "],\"buttons\":[");	
	if(ret!=ESP_OK) return ret;
	
	
	int where;
	button_settings_command_t *cmd;
	bool first;
	for(int idBtn=0;idBtn<NUM_OF_BUTTONS;idBtn++){
		memset(buf, 0, 200);
		
		if(idBtn>0){
			ret=httpd_resp_sendstr_chunk(req, ",");
			if(ret!=ESP_OK) return ret;
		}

		ret=httpd_resp_sendstr_chunk(req, "[");	
		if(ret!=ESP_OK) return ret;
		
		first=true;
		for(int idCmd=0;idCmd<SETTINGS_PER_BUTTON;idCmd++){
			cmd=&settings.buttons[idBtn][idCmd];
			if(cmd->eventType){
				if(!first){
					ret=httpd_resp_sendstr_chunk(req, ",");
					if(ret!=ESP_OK) return ret;
				}
				// genero il json con le info
				where=(cmd->where[0]<<8)|cmd->where[1];
				sprintf(buf,"{\"e\":\"%i\",\"c\":[\"%02x\",\"%02x\"],\"w\":\"%04x\",\"i\":\"%02x\"}"
					,cmd->eventType
					,cmd->command[0]
					,cmd->command[1]
					,where
					,cmd->interface
				);
				ret = httpd_resp_sendstr_chunk(req, buf);
				if(ret!=ESP_OK) return ret;
				first=false;
			}
		}
		
		ret=httpd_resp_sendstr_chunk(req, "]");	
		if(ret!=ESP_OK) return ret;
		
	}

	ret=httpd_resp_sendstr_chunk(req, "]}");
	if(ret!=ESP_OK) return ret;

	ret = httpd_resp_send_chunk(req, buf,0);
	if(ret!=ESP_OK) return ret;


	return ret;
}















