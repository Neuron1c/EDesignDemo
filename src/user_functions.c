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
static uint16_t count = 0;
extern uint16_t scrollCount;
extern uint8_t scrollFlag;
extern uint8_t tempFlag;
extern uint8_t recieveflag;
extern uint8_t sendflag;
extern uint8_t secFlag;

static uint16_t finalTemp;
static uint8_t response[RX_BUF_LEN];
static int mode = 0;
static int display = 0;
static int offset = 0;
int D = 0;
int flag = 0;
extern uint8_t voltage, current1, current2;
extern uint8_t date[6];
static uint8_t switchState;
static uint8_t tempState;
static uint16_t logCount;

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
void read(uint16_t addr, uint8_t len);
void writeLog();
void writeMem(uint16_t addr);
void readLog(uint16_t logNum);
uint8_t testDate(uint8_t arr[]);

char populate(char recieved) {

	if (recieved == '$') {
		record[count] = recieved;
		count++;
	} else if (recieved == '\x0a') {
		if (record[count - 1] == '\x0d') {
			if (!flag) {
				record[count] = recieved;
				record[count + 1] = '\0';
				processMSG();
			}
		}
		strcpy(record, "");
		count = 0;
		flag = 0;
	} else if (flag) {

	} else if (count == 0 && recieved != '$') {

		R_UART1_Send("error\x0d\x0a", 7);
		flag = 1;
	}

	else {
		record[count] = recieved;
		count++;
	}

	return recieved;
}

void processMSG() {
	char ack[3];
	char str[30];
	if (errorTest()) {

		switch (record[1]) {
		case '0':
			if (mode == 1) {
				ack[0] = '0';
				mode = 0;
			}

			else {
				ack[0] = '3';
			}

			ack[1] = '\x0d';
			ack[2] = '\x0a';
			strcpy(response, "$0");
			strcat(response, ack);
			recieveInstruction(7);
			count = 5;
			break;

		case '1':
			if (mode == 0) {
				ack[0] = '0';
				mode = 1;
			}

			else {
				ack[0] = '3';
			}

			ack[1] = '\x0d';
			ack[2] = '\x0a';
			strcpy(response, "$1");
			strcat(response, ack);
			recieveInstruction(6);
			count = 5;
			break;

		case '2':
			strcpy(mem, &record[2]);
			strcpy(response, "$20\x0d\x0a");
			count = 5;
			break;

		case '3':
			strcpy(response, "$3019119038\x0d\x0a");
			count = 13;
			break;

		case 'D':
			if (strlen(mem) <= 32) {

				strcpy(response, "$D0\x0d\x0a");
				count = 5;

				strcpy(display_string, mem);
				displayLCD(1);
			} else {
				writeByteLcd(LCD_CTRL_WR, LCD_CLEAR);
				delayNoInt(1640);

				writeByteLcd(LCD_CTRL_WR, LCD_HOME_L1);
				delayNoInt(1640);
				strcpy(response, "$D3\x0d\x0a");
				count = 5;
			}
			break;

		case 'L':
			if (record[2] == '0') {
				P4_bit.no1 = 0;
				P4_bit.no2 = 0;
				P4_bit.no3 = 0;
				switchState = 0;
				strcpy(response, "$L0\x0d\x0a");
				count = 5;
			} else if (record[2] == '1') {
				P4_bit.no1 = 0;
				P4_bit.no2 = 0;
				P4_bit.no3 = 1;
				switchState = 1;
				strcpy(response, "$L0\x0d\x0a");
				count = 5;
			} else if (record[2] == '2') {
				P4_bit.no1 = 1;
				P4_bit.no2 = 0;
				P4_bit.no3 = 0;
				switchState = 2;
				strcpy(response, "$L0\x0d\x0a");
				count = 5;
			} else if (record[2] == '3') {
				P4_bit.no1 = 0;
				P4_bit.no2 = 1;
				P4_bit.no3 = 0;
				switchState = 3;
				strcpy(response, "$L0\x0d\x0a");
				count = 5;
			} else if (record[2] == '4') {
				P4_bit.no1 = 1;
				P4_bit.no2 = 1;
				P4_bit.no3 = 0;
				switchState = 4;
				strcpy(response, "$L0\x0d\x0a");
				count = 5;
			} else {
				strcpy(response, "$L2\x0d\x0a");
				count = 5;
			}
			break;
		case 'E':
			sprintf(str, "$E0%02d,%02d,%0d\x0d\x0a", voltage, current2,
					current1);

			//				  sprintf(str, "%d", voltage);

			strcpy(response, str);
			count = strlen(str);

			break;

		case 'B':
			setRTC();
			break;
		case 'C':
			getRTC();
			sprintf(response, "$C020%d,%d,%d,%d,%d,%d\x0d\x0a", date[0], date[1], date[2],
					date[3], date[4], date[5]);
			count = strlen(response);
			break;
		case 'I':
			sprintf(response, "$I0%03d\x0d\x0a", finalTemp);
			count = strlen(response);
			break;
		case 'K':
			if (1) {
			}
			uint16_t addr = (record[2] - '0') * 1000 + (record[3] - '0') * 100
					+ (record[4] - '0') * 10 + (record[5] - '0');
			uint8_t len = record[7] - '0';
			len = len + (record[6] - '0') * 10;
			if (len > 32) {
				sprintf(response, "$K%d", len);
			} else {
				read(addr, len);
			}
			break;
		case 'J':
			if (1) {
			}
			uint16_t address = (record[2] - '0') * 1000
					+ (record[3] - '0') * 100 + (record[4] - '0') * 10
					+ (record[5] - '0');
			writeMem(address);
			break;
		case 'M':
			if (1) {

			}
			int i = 2;
			uint16_t log = 0;
			while (record[i] != '\x0d') {
				log = log * 10;
				log += record[i] - '0';
				i++;
			}
			readLog(log);
			break;

		case 'N':

			sprintf(response, "$N02427\x0d\x0a");
			count = 9;
			break;
		default:
			sprintf(response, "$%c1\x0d\x0a", record[1]);
			count = 5;
//				 strcpy(response, "$");
//				 strcat(response, &record[1]);
//				 strcat(response, "1\x0d\x0a"); count = 5;

		}
	}

	R_UART1_Send(response, strlen(response));
	count = 0;
}

int errorTest() {

//	if(record[1] == '0' || record[1] == '1' ||record[1] == '3'||record[1] == '2' || record[1] == 'D'){	//INVALID COMMAND
//	}
//	else{
//		char validChar[] = {'$',record[1],'1','\x0d','\x0a'};
//		strcpy(record, validChar);
//		count = 5;
//		return 0;
//	}

	if (record[strlen(record) - 1] != '\x0a') {	//INVALID PARAMETERS
		char validChar[] = { '$', record[1], '2', '\x0d', '\x0a' };
		strcpy(record, validChar);
		count = 5;
		return 0;
	}

	return 1;
}
//reset >= 1 clears screen and starts over
//reset = 0 just keep going

void displayLCD(int reset) {

	if (reset) {
		offset = 0;
		display = 1;
		scrollCount = 0;
	}

	if (display) {

		int i;

		writeByteLcd(LCD_CTRL_WR, LCD_CLEAR);
		delayNoInt(1640);

		writeByteLcd(LCD_CTRL_WR, LCD_HOME_L1);
		delayNoInt(1640);

		for (i = offset; i < strlen(display_string) - 2; i++) {

			if (i == (8 + offset)) {
				writeByteLcd(LCD_CTRL_WR, LCD_HOME_L2);
				delayNoInt(1640);
			}

			writeByteLcd(LCD_DATA_WR, display_string[i]);
			delayNoInt(40);

		}
		offset++;

	}

	if (offset >= (strlen(display_string) - 17)
			|| strlen(display_string) <= 18) {
		display = 0;
	}

}

void collectADC() {

	ADS = _00_AD_INPUT_CHANNEL_0;
	R_ADC_Start();
	while (ADCS)
		;
	R_ADC_Start();
	while (ADCS)
		;

	R_ADC_Get_Result_8bit(&voltage);
	voltage = voltage >> 1;

	ADS = _01_AD_INPUT_CHANNEL_1;
	R_ADC_Start();
	while (ADCS)
		;
	R_ADC_Start();
	while (ADCS)
		;

	R_ADC_Get_Result_8bit(&current1);
	current1 = current1 >> 2;

	ADS = _02_AD_INPUT_CHANNEL_2;
	R_ADC_Start();
	while (ADCS)
		;
	R_ADC_Start();
	while (ADCS)
		;

	R_ADC_Get_Result_8bit(&current2);
	current2 = current2 >> 2;

}

void setRTC() {
	uint8_t BCDdate[6] = { 0, 0, 0, 0, 0, 0 };
	date[0] = 0;
	date[1] = 0;
	date[2] = 0;
	date[3] = 0;
	date[4] = 0;
	date[5] = 0;
	uint8_t count = 4;
	int dateCount = 0;
	rtc_counter_value_t theDate;

	while (record[count] != '\r') {

		BCDdate[dateCount] += record[count] - '0';
		date[dateCount] += record[count] - '0';
		count++;
		if (record[count] == ',') {
			dateCount++;
			count++;
		} else if (record[count] == '\r') {

		} else {
			BCDdate[dateCount] = BCDdate[dateCount] << 4;
			date[dateCount] = date[dateCount] * 10;
		}

	}

	if (testDate(date)) {

		theDate.year = BCDdate[0];
		theDate.month = BCDdate[1];
		theDate.day = BCDdate[2];
		theDate.hour = BCDdate[3];
		theDate.min = BCDdate[4];
		theDate.sec = BCDdate[5];

		count = 5;
		strcpy(response, "$B0\x0d\x0a");

		R_RTC_Set_CounterValue(theDate);
	} else {
		char str[4];
		uint8_t ack = 0;
		sprintf(str, "$B2\x0d\x0a");
		count = strlen(str);
		strcpy(response, str);

	}

}

void getRTC() {
	rtc_counter_value_t theDate;

	R_RTC_Get_CounterValue(&theDate);

	date[0] = theDate.year;
	date[1] = theDate.month;
	date[2] = theDate.day;
	date[3] = theDate.hour;
	date[4] = theDate.min;
	date[5] = theDate.sec;

	BCDtoDEC(date);


}

void BCDtoDEC(uint8_t arr[]) {

	uint8_t upper, lower;
	int i;
	for (i = 0; i < 6; i++) {
		lower = arr[i] & 0x0f;
		upper = (arr[i] & 0xf0) >> 4;

		arr[i] = upper * 10 + lower;
	}
}

// arr format [year,month,day,hour,minute,second]
uint8_t testDate(uint8_t arr[]) {

	int daysInMonth[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

	if (arr[1] > 12 || arr[1] == 0) {
		return 0;
	} else if (arr[2] > daysInMonth[arr[1]] || arr[2] == 0) {
		return 0;
	} else if (arr[3] > 23) {
		return 0;
	} else if (arr[4] > 59) {
		return 0;
	} else if (arr[5] > 59) {
		return 0;
	} else {
		return 1;

	}

}

void checkFlags() {
	if (scrollFlag) {
		scrollFlag = 0;
		displayLCD(0);
	}
	if (tempFlag) {
		tempFlag = 0;
		P1_bit.no7 = !P1_bit.no7;

		if (!P1_bit.no7) {
			setTemp();
		}
	}

	if (secFlag) {
		secFlag = 0;
		collectADC();
		writeLog();
	}

	if (mode == 0) {
		if (tempState == 0) {
			P4_bit.no1 = 1;
			P4_bit.no2 = 0;
			P4_bit.no3 = 0;
			switchState = 2;
			if (finalTemp > 27) {
				tempState = 1;
			}
		} else if (tempState == 1) {
			P4_bit.no1 = 0;
			P4_bit.no2 = 1;
			P4_bit.no3 = 0;
			switchState = 3;
			if (finalTemp < 24) {
				tempState = 1;
			}
		}
	}

}

void setTemp() {
	uint16_t temp = 4096 - TCR01;
	finalTemp = temp / 16;
	finalTemp -= 50;
	R_TAU0_Channel1_Stop();
	R_TAU0_Channel1_Start();

}

void writeMem(uint16_t addr) {
	uint8_t send[35];
	uint8_t recieve[3];

	send[0] = 6;

	P1_bit.no5 = 0;
	R_CSI00_Send_Receive(send, 1, recieve);

	while (recieveflag) {
	}
	recieveflag = 1;
	P1_bit.no5 = 1;
	delayNoInt(10);

	P1_bit.no5 = 0;
	send[0] = 2;
	send[1] = addr >> 4;
	send[2] = addr & 0x0F;
	sprintf(&send[3], "%s", mem);

	R_CSI00_Send_Receive(send, strlen(mem) + 3, recieve);

	while (recieveflag) {
	}
	recieveflag = 1;
	P1_bit.no5 = 1;
	delayNoInt(10);

	send[0] = 4;

	P1_bit.no5 = 0;
	R_CSI00_Send_Receive(send, 1, recieve);

	while (recieveflag) {
	}
	recieveflag = 1;
	P1_bit.no5 = 1;
	delayNoInt(10);

	sprintf(response, "$J0\x0d\x0a");
	count = 5;
}

void writeLog() {
	uint8_t send[35];
	uint8_t recieve[3];

	send[0] = 6;

	P1_bit.no5 = 0;
	R_CSI00_Send_Receive(send, 1, recieve);

	while (recieveflag) {
	}
	recieveflag = 1;
	P1_bit.no5 = 1;
	delayNoInt(10);

	getRTC();
	count = 0;

	send[0] = 2;
	send[1] = logCount >> 4;
	send[2] = logCount & 0x0F;
	send[3] = date[0];
	send[4] = date[1];
	send[5] = date[2];
	send[6] = date[3];
	send[7] = date[4];
	send[8] = date[5];
	send[9] = switchState;
	send[10] = finalTemp;
	send[11] = voltage;
	send[12] = current1;
	send[13] = 24;
	send[14] = 27;
	send[15] = '\n';
	send[16] = 0;
	send[17] = 0;
	send[18] = 0;
	send[19] = 0;

	P1_bit.no5 = 0;
	R_CSI00_Send_Receive(send, 19, recieve);

	while (recieveflag) {
	}
	recieveflag = 1;
	P1_bit.no5 = 1;
	delayNoInt(10);

	send[0] = 4;

	P1_bit.no5 = 0;
	R_CSI00_Send_Receive(send, 1, recieve);

	while (recieveflag) {
	}
	sendflag = 1;
	recieveflag = 1;
	P1_bit.no5 = 1;
	delayNoInt(10);

	if (logCount <= 8192) {
		logCount += 16;
	}
}

void read(uint16_t addr, uint8_t len) {
	uint8_t send[10];
	uint8_t recieve[35];

	send[0] = 3;
	send[1] = addr >> 4;
	send[2] = addr & 0x0F;
	P1_bit.no5 = 0;
	recieveflag = 1;
	sendflag = 1;
	R_CSI00_Send_Receive(send, len + 3, recieve);

	while (recieveflag || sendflag) {
	}

	recieveflag = 1;
	sendflag = 1;
	P1_bit.no5 = 1;
	count = len + 3;
	sprintf(response, "$K0%s\x0d\x0a", &recieve[3]);
}

void readLog(uint16_t logNum) {
	uint8_t send[10];
	uint8_t recieve[35];

	logNum = (logNum - 1) * 16;
	send[0] = 3;
	send[1] = logNum >> 4;
	send[2] = logNum & 0x0F;
	P1_bit.no5 = 0;
	recieveflag = 1;
	sendflag = 1;
	R_CSI00_Send_Receive(send, 17, recieve);

	while (recieveflag || sendflag) {
	}
	recieveflag = 1;
	sendflag = 1;
	P1_bit.no5 = 1;

	if (logNum > logCount) {
		sprintf(response, "$M2\x0d\x0a");
		count = strlen(response);
	} else {
		sprintf(response, "$M020%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\x0d\x0a",
				recieve[3], recieve[4], recieve[5], recieve[6], recieve[7],
				recieve[8], recieve[9], recieve[10], recieve[11], recieve[12],
				recieve[13], recieve[14]);
		count = strlen(response);
	}

}
