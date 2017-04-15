/*
 * user_functions.c
 *
 *  Created on: 15 Feb 2017
 *      Author: 19119038
 */
#include "r_cg_macrodriver.h"
#include "r_cg_cgc.h"
#include "r_cg_port.h"
#include "r_cg_serial.h"
#include "r_cg_wdt.h"
#include "r_cg_userdefine.h"
#include "debounce.h"
#include <string.h>
#include <stdio.h>

extern uint8_t record[RX_BUF_LEN];
extern uint8_t mem[RX_BUF_LEN];
extern int count;
extern int interval_timer_count;
static uint8_t response[RX_BUF_LEN];
static int mode = 0;
static int display = 0;
static int offset = 0;
int D = 0;
int flag = 0;
uint8_t voltage, current1, current2;


void processMSG();
int errorTest();
void displayLCD();
void readRTC();
void setRTC();
void collectADC();
uint8_t testDate(uint8_t arr[]);


char populate(char recieved){

	if(recieved == '$'){
		record[count] = recieved;
		count++;
	}
	else if(recieved == '\x0a'){
		if(record[count - 1] == '\x0d'){
			if(!flag){
				record[count] = recieved;
				record[count + 1] = '\0';
				processMSG();
			}
		}
		strcpy(record,"");
		count = 0;
		flag = 0;
	}
	else if(flag){

	}
	else if(count == 0 && recieved != '$'){

		R_UART1_Send("error\x0d\x0a",7);
		flag = 1;
	}

	else{
		record[count] = recieved;
		count++;
	}



	return recieved;
}

void processMSG(){
	char ack[3];
	char str[30];
	if(errorTest()){

		switch(record[1]){
		case '0': if(mode == 1){ack[0] = '0'; mode = 0;}
				  else{ack[0] = '3';}
				  ack[1] = '\x0d';
				  ack[2] = '\x0a';
				  strcpy(response, "$0");
				  strcat(response, ack);
				  count = 5; break;


		case '1': if(mode == 0){ack[0] = '0'; mode = 1;}
		  	  	  else{ack[0] = '3';}
				  ack[1] = '\x0d';
				  ack[2] = '\x0a';
				  strcpy(response, "$1");
				  strcat(response, ack);
				  count = 5; break;

		case '2': strcpy(mem, &record[2]);
				  strcpy(response, "$20\x0d\x0a"); count = 5; break;

		case '3': strcpy(response, "$3019119038\x0d\x0a"); count = 13; break;

		case 'D': if(strlen(mem) <= 32){

					strcpy(response, "$D0\x0d\x0a"); count = 5;
					offset = 0; display = 1; interval_timer_count = 0; R_IT_Start(); displayLCD();
				  }
				  else{
				    writeByteLcd(LCD_CTRL_WR, LCD_CLEAR);
					delayNoInt(1640);

					writeByteLcd(LCD_CTRL_WR, LCD_HOME_L1);
					delayNoInt(1640);
					strcpy(response, "$D3\x0d\x0a"); count = 5;
				  }
				  break;

		case 'L': if(record[2] == '0'){
					  P4_bit.no1 = 0;
					  P4_bit.no2 = 0;
					  P4_bit.no3 = 0;
					  strcpy(response, "$L0\x0d\x0a"); count = 5;
				  }
				  else if(record[2] == '1'){
					  P4_bit.no1 = 0;
					  P4_bit.no2 = 0;
					  P4_bit.no3 = 1;
					  strcpy(response, "$L0\x0d\x0a"); count = 5;
				  }
				  else if(record[2] == '2'){
					  P4_bit.no1 = 1;
					  P4_bit.no2 = 0;
					  P4_bit.no3 = 0;
					  strcpy(response, "$L0\x0d\x0a"); count = 5;
				  }
				  else if(record[2] == '3'){
					  P4_bit.no1 = 0;
					  P4_bit.no2 = 1;
					  P4_bit.no3 = 0;
					  strcpy(response, "$L0\x0d\x0a"); count = 5;
				  }
				  else if(record[2] == '4'){
					  P4_bit.no1 = 1;
					  P4_bit.no2 = 1;
					  P4_bit.no3 = 0;
					  strcpy(response, "$L0\x0d\x0a"); count = 5;
				  }
				  else{
					  strcpy(response, "$L2\x0d\x0a"); count = 5;
				  }
				  break;
		case 'E':

				 collectADC();
				 sprintf(str, "$E0%02d,0,%02d\x0d\x0a", voltage, current2);


//				  sprintf(str, "%d", voltage);

			      strcpy(response, str); count = strlen(str);

				  break;
		 case'B': setRTC();
			      break;


		default:
				 strcpy(response, "$");
				 strcat(response, &record[1]);
				 strcat(response, "1\x0d\x0a"); count = 5;

		}
	}

	R_UART1_Send(response, count);
}

int errorTest(){

//	if(record[1] == '0' || record[1] == '1' ||record[1] == '3'||record[1] == '2' || record[1] == 'D'){	//INVALID COMMAND
//	}
//	else{
//		char validChar[] = {'$',record[1],'1','\x0d','\x0a'};
//		strcpy(record, validChar);
//		count = 5;
//		return 0;
//	}

	if(record[strlen(record) - 1] != '\x0a'){	//INVALID PARAMETERS
		char validChar[] = {'$',record[1],'2','\x0d','\x0a'};
		strcpy(record, validChar);
		count = 5;
		return 0;
	}

	return 1;
}

void displayLCD(){

if(display){

	int i;

	writeByteLcd(LCD_CTRL_WR, LCD_CLEAR);
	delayNoInt(1640);

	writeByteLcd(LCD_CTRL_WR, LCD_HOME_L1);
	delayNoInt(1640);

	for(i = offset; i < strlen(mem) - 2; i++){

		if(i == (8 + offset)){
			writeByteLcd(LCD_CTRL_WR, LCD_HOME_L2);
			delayNoInt(1640);
		}

		writeByteLcd(LCD_DATA_WR, mem[i]);
		delayNoInt(40);

	}
	offset++;

}

if(offset >= (strlen(mem) - 17) || strlen(mem) <= 18 ){
	display = 0;
	R_IT_Stop();
}

}

void collectADC(){

	 ADS = _00_AD_INPUT_CHANNEL_0;
	 R_ADC_Start();
	 while(ADCS);
	 R_ADC_Start();
	 while(ADCS);

	 R_ADC_Get_Result_8bit(&voltage);
	 voltage = voltage >> 1;

	 ADS = _02_AD_INPUT_CHANNEL_2;
	 R_ADC_Start();
	 while(ADCS);
	 R_ADC_Start();
	 while(ADCS);

	 R_ADC_Get_Result_8bit(&current2);
	 current2 = current2 >> 2;

}

void setRTC(){
	uint8_t date[] = {0,0,0,0,0,0};
	count = 4;
	int dateCount = 0;
	rtc_counter_value_t theDate;

	while(record[count] != '\r'){

		date[dateCount] += record[count] - '0';
		count++;
		if(record[count] == ','){
			dateCount++;
			count++;
		}
		else if(record[count] == '\r'){

		}
		else{
			date[dateCount] = date[dateCount]<<4;
		}

	}

	theDate.year = date[0];
	theDate.month = date[1];
	theDate.day = date[2];
	theDate.hour = date[3];
	theDate.min = date[4];
	theDate.sec = date[5];

	char str[4];
	uint8_t ack = 0;
	sprintf(str, "$%d\x0d\x0a", date[0]); count = strlen(str);
	strcpy(response, str);

	if(ack == 0){
		R_RTC_Set_CounterValue(theDate);
	}
}

uint8_t testDate(uint8_t arr[]){


	if(arr[0] > 153){
		return 2;
	}
	if(arr[1] > 18){
		return 2;
	}
	if(arr[2] > 49){
		return 2;
	}
	if(arr[3] > 36){
		return 2;
	}
	if(arr[4] > 96){
		return 2;
	}
	if(arr[5] > 96){
		return 2;
	}


	return 0;
}
