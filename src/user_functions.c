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
#include <stdlib.h>

extern uint8_t record[RX_BUF_LEN];
extern uint8_t mem[RX_BUF_LEN];
extern uint8_t display_string[RX_BUF_LEN];
extern int count;
extern uint16_t scrollCount;
extern uint8_t scrollFlag;
extern uint8_t tempFlag;

static uint16_t finalTemp;
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
void getRTC();
void BCDtoDEC(uint8_t arr[]);
void collectADC();
void checkFlags();
void measureTemp();
void setTemp();
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
		case '0': if(mode == 1){
					 ack[0] = '0'; mode = 0;
				  }

				  else{
					  ack[0] = '3';
				  }

				  ack[1] = '\x0d';
				  ack[2] = '\x0a';
				  strcpy(response, "$0");
				  strcat(response, ack);
				  recieveInstruction(7);
				  count = 5; break;


		case '1': if(mode == 0){
					  ack[0] = '0'; mode = 1;
				  }

		  	  	  else{
		  	  		  ack[0] = '3';
		  	  	  }

				  ack[1] = '\x0d';
				  ack[2] = '\x0a';
				  strcpy(response, "$1");
				  strcat(response, ack);
				  recieveInstruction(6);
				  count = 5; break;

		case '2': strcpy(mem, &record[2]);
				  strcpy(response, "$20\x0d\x0a"); count = 5; break;

		case '3': strcpy(response, "$3019119038\x0d\x0a"); count = 13; break;

		case 'D': if(strlen(mem) <= 32){

					strcpy(response, "$D0\x0d\x0a"); count = 5;

					strcpy(display_string,mem);
					displayLCD(1);
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
					 sprintf(str, "$E0%02d,%02d,%0d\x0d\x0a", voltage, current2, current1);

	//				  sprintf(str, "%d", voltage);

					  strcpy(response, str); count = strlen(str);

					  break;

	     case 'B':    setRTC();
					  break;
		 case 'C':	  getRTC();
		 	 	 	  break;
		 case 'I':	  sprintf(response, "$I0%d\x0d\x0a", finalTemp);
		 	 	 	  count = strlen(response);
		 	 	 	  break;


		default:
				 sprintf(response, "$%c1\x0d\x0a", record[1]);
				 count = 5;
//				 strcpy(response, "$");
//				 strcat(response, &record[1]);
//				 strcat(response, "1\x0d\x0a"); count = 5;

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
//reset >= 1 clears screen and starts over
//reset = 0 just keep going

void displayLCD(int reset){

if(reset){
	offset = 0;
	display = 1;
	scrollCount = 0;
}

if(display){

	int i;

	writeByteLcd(LCD_CTRL_WR, LCD_CLEAR);
	delayNoInt(1640);

	writeByteLcd(LCD_CTRL_WR, LCD_HOME_L1);
	delayNoInt(1640);

	for(i = offset; i < strlen(display_string) - 2; i++){

		if(i == (8 + offset)){
			writeByteLcd(LCD_CTRL_WR, LCD_HOME_L2);
			delayNoInt(1640);
		}

		writeByteLcd(LCD_DATA_WR, display_string[i]);
		delayNoInt(40);

	}
	offset++;

}

if(offset >= (strlen(display_string) - 17) || strlen(display_string) <= 18 ){
	display = 0;
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

	 ADS = _01_AD_INPUT_CHANNEL_1;
	 R_ADC_Start();
	 while(ADCS);
	 R_ADC_Start();
	 while(ADCS);

	 R_ADC_Get_Result_8bit(&current1);
	 current1 = current1 >> 2;

	 ADS = _02_AD_INPUT_CHANNEL_2;
	 R_ADC_Start();
	 while(ADCS);
	 R_ADC_Start();
	 while(ADCS);

	 R_ADC_Get_Result_8bit(&current2);
	 current2 = current2 >> 2;

}

void setRTC(){
	uint8_t date[6] = {0,0,0,0,0,0};
	uint8_t actualDate[6] = {0,0,0,0,0,0};
	count = 4;
	int dateCount = 0;
	rtc_counter_value_t theDate;

	while(record[count] != '\r'){

		date[dateCount] += record[count] - '0';
		actualDate[dateCount] += record[count] - '0';
		count++;
		if(record[count] == ','){
			dateCount++;
			count++;
		}
		else if(record[count] == '\r'){

		}
		else{
			date[dateCount] = date[dateCount]<<4;
			actualDate[dateCount] = actualDate[dateCount]*10;
		}

	}

	if(testDate(actualDate)){

		theDate.year = date[0];
		theDate.month = date[1];
		theDate.day = date[2];
		theDate.hour = date[3];
		theDate.min = date[4];
		theDate.sec = date[5];

		count = 5;
		strcpy(response, "$B0\x0d\x0a");

		R_RTC_Set_CounterValue(theDate);
	}
	else{
		char str[4];
		uint8_t ack = 0;
		sprintf(str, "$B2\x0d\x0a");
		count = strlen(str);
		strcpy(response, str);

	}

}

void getRTC(){
	rtc_counter_value_t theDate;
	uint8_t date[] = {0,0,0,0,0,0};



	R_RTC_Get_CounterValue(&theDate);

	date[0] = theDate.year;
	date[1] = theDate.month;
	date[2] = theDate.day;
	date[3] = theDate.hour;
	date[4] = theDate.min;
	date[5] = theDate.sec;

	BCDtoDEC(date);

	char *str;
	str = (char *)malloc(32);
	sprintf(str, "$C020%d,%d,%d,%d,%d,%d\x0d\x0a", date[0], date[1], date[2], date[3], date[4], date[5]);
//	free(date);
	count = strlen(str);
	strcpy(response, str);
	free(str);

}

void BCDtoDEC(uint8_t arr[]){

	uint8_t len =  6;
	uint8_t upper, lower;
	int i;
	for(i = 0; i < len; i++){
		lower = arr[i] & 0x0f;
		upper = (arr[i] & 0xf0)>>4;

		arr[i] = upper*10 + lower;
	}
}

// arr format [year,month,day,hour,minute,second]
uint8_t testDate(uint8_t arr[]){

	int daysInMonth[12] = {31,28,31,30,31,30,31,31,30,31,30,31};

	if(arr[1] > 12){
		return 0;
	}
	else if(arr[2] > daysInMonth[arr[1]]){
		return 0;
	}
	else if(arr[3] > 23){
		return 0;
	}
	else if(arr[4] > 59){
		return 0;
	}
	else if(arr[5] > 59){
		return 0;
	}
	else{
		return 1;

	}

}

void checkFlags(){
	 if(scrollFlag){
		 scrollFlag = 0;
		 displayLCD(0);
	 }
	 if(tempFlag){
		 tempFlag = 0;
		 P1_bit.no7 = !P1_bit.no7;

		 if(!P1_bit.no7){
			 setTemp();
		 }
	 }
}

void setTemp(){
	uint16_t temp = 4096 - TCR01;
	finalTemp = temp/16;
	finalTemp -= 50;
	R_TAU0_Channel1_Stop();
	R_TAU0_Channel1_Start();

}
