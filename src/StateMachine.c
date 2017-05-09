/*
 * StateMachine.c
 *
 *  Created on: 15 Apr 2017
 *      Author: Neuron1c
 */

#include "r_cg_userdefine.h"
#include <string.h>
#include <stdio.h>

static int state = 0;
extern uint8_t display_string[RX_BUF_LEN];
extern uint8_t record[RX_BUF_LEN];
extern uint8_t voltage, current1;
extern uint8_t date[6];
static uint8_t newDate[6];

//Recieve instruction, value from 1 to 7
//1 = right button (accept/okay) (pin 2)
//2 = middle button
//3 = down button (pin 4)
//4 = up button (pin 3)
//5 = left button (back/cancel) (pin 5)
//6 = switch to test mode
//7 = switch to normal mode

void recieveInstruction(int instr) {
	switch (state) {

	//---------------------------------------------------- normal mode

	case 0:
		if (instr == 3)
			state = 1;
		else if (instr == 4)
			state = 1;
		else if (instr == 6)
			state = 5;
		break;
	case 1:
		if (instr == 3)
			state = 0;
		else if (instr == 4)
			state = 0;
		else if (instr == 1) {
			state = 2;
		}

		else if (instr == 6)
			state = 5;
		break;
	case 2:
		if (instr == 1) {
			state = 13;
			strcpy(newDate, date);
		} else if (instr == 4) {
			state = 12;
		} else if (instr == 3) {
			state = 3;
		} else if (instr == 5) {
			state = 1;
		} else if (instr == 6) {
			state = 5;
		}
		break;
	case 3:
		if (instr == 1) {
			state = 14;
			strcpy(newDate, date);
		} else if (instr == 4) {
			state = 2;
		} else if (instr == 3) {
			state = 4;
		} else if (instr == 5) {
			state = 1;
		} else if (instr == 6) {
			state = 5;
		}
		break;
	case 4:
		if (instr == 1) {
			state = 15;
			strcpy(newDate, date);
		} else if (instr == 4) {
			state = 3;
		} else if (instr == 3) {
			state = 10;
		} else if (instr == 5) {
			state = 1;
		} else if (instr == 6) {
			state = 5;
		}
		break;
	case 10:
		if (instr == 1) {
			state = 16;
			strcpy(newDate, date);
		} else if (instr == 4) {
			state = 4;
		} else if (instr == 3) {
			state = 11;
		} else if (instr == 5) {
			state = 1;
		} else if (instr == 6) {
			state = 5;
		}
		break;
	case 11:
		if (instr == 1) {
			state = 17;
			strcpy(newDate, date);
		} else if (instr == 4) {
			state = 10;
		} else if (instr == 3) {
			state = 12;
		} else if (instr == 5) {
			state = 1;
		} else if (instr == 6) {
			state = 5;
		}
		break;
	case 12:
		if (instr == 1) {
			state = 18;
			strcpy(newDate, date);
		} else if (instr == 4) {
			state = 11;
		} else if (instr == 3) {
			state = 2;
		} else if (instr == 5) {
			state = 1;
		} else if (instr == 6) {
			state = 5;
		}
		break;
	case 13:
		if (instr == 1) { //accept command
			sprintf(record, "$B20%02d,%d,%d,%d,%d,%d\r\n", newDate[0],
					newDate[1], newDate[2], newDate[3], newDate[4], newDate[5]);
			setRTC();
			state = 2;
		} else if (instr == 5) {
			state = 2;
		}
		break;
	case 14:
		if (instr == 1) { //accept command
			sprintf(record, "$B20%02d,%d,%d,%d,%d,%d\r\n", newDate[0],
					newDate[1], newDate[2], newDate[3], newDate[4], newDate[5]);
			setRTC();
			state = 3;
		} else if (instr == 5) {
			state = 3;
		}
		break;
	case 15:
		if (instr == 1) { //accept command
			sprintf(record, "$B20%02d,%d,%d,%d,%d,%d\r\n", newDate[0],
					newDate[1], newDate[2], newDate[3], newDate[4], newDate[5]);
			setRTC();
			state = 4;
		} else if (instr == 5) {
			state = 4;
		}
		break;
	case 16:
		if (instr == 1) { //accept command
			sprintf(record, "$B20%02d,%d,%d,%d,%d,%d\r\n", newDate[0],
					newDate[1], newDate[2], newDate[3], newDate[4], newDate[5]);
			setRTC();
			state = 10;
		} else if (instr == 5) {
			state = 10;
		}
		break;
	case 17:
		if (instr == 1) { //accept command
			sprintf(record, "$B20%02d,%d,%d,%d,%d,%d\r\n", newDate[0],
					newDate[1], newDate[2], newDate[3], newDate[4], newDate[5]);
			setRTC();
			state = 11;
		} else if (instr == 5) {
			state = 11;
		}
		break;
	case 18:
		if (instr == 1) { //accept command
			sprintf(record, "$B20%02d,%d,%d,%d,%d,%d\r\n", newDate[0],
					newDate[1], newDate[2], newDate[3], newDate[4], newDate[5]);
			setRTC();
			state = 12;
		} else if (instr == 5) {
			state = 12;
		}
		break;

		//---------------------------------------------------- test mode

	case 5:
		if (instr == 3)
			state++;
		else if (instr == 4)
			state = 9;
		else if (instr == 7)
			state = 0;
		break;
	case 6:
		if (instr == 3)
			state++;
		else if (instr == 4)
			state--;
		else if (instr == 7)
			state = 0;
		break;
	case 7:
		if (instr == 3)
			state++;
		else if (instr == 4)
			state--;
		else if (instr == 7)
			state = 0;
		break;
	case 8:
		if (instr == 3)
			state++;
		else if (instr == 4)
			state--;
		else if (instr == 7)
			state = 0;
		break;
	case 9:
		if (instr == 3)
			state = 5;
		else if (instr == 4)
			state--;
		else if (instr == 7)
			state = 0;
		break;
	}

	//STATE FUNCTIONS

	/*
	 * Normal mode functions
	 *
	 * 0 - adjust date time
	 * 1 - measure open circuit voltage and short circuit current
	 * 2 - Set Year
	 * 3 - Set Month
	 * 4 - Set Day
	 * 10 - Set Hour
	 * 11 - Set Minutes
	 * 12 - Set Seconds
	 * 13 - Year:
	 * 14 - Month:
	 * 15 - Day:
	 * 16 - Hour:
	 * 17 - Minute:
	 * 18 - Second:
	 *
	 *
	 *
	 * Test mode functions
	 *
	 * 5 - $L0
	 * 6 - $L1
	 * 7 - $L2
	 * 8 - $L3
	 * 9 - $L4
	 */

	switch (state) {
	case 0:
		sprintf(display_string, "SP:Voc=%d mV, Isc=%d mA\x0d\x0a", voltage,
				current1);
		displayLCD(1);
		break;
	case 1:
		strcpy(display_string, "Set date/time\x0d\x0a");
		displayLCD(1);
		break;
	case 2:
		strcpy(display_string, "Set Year\x0d\x0a");
		displayLCD(1);
		break;
	case 3:
		strcpy(display_string, "Set Month\x0d\x0a");
		displayLCD(1);
		break;
	case 4:
		strcpy(display_string, "Set Day\x0d\x0a");
		displayLCD(1);
		break;
	case 10:
		strcpy(display_string, "Set Hour\x0d\x0a");
		displayLCD(1);
		break;
	case 11:
		strcpy(display_string, "Set Minute\x0d\x0a");
		displayLCD(1);
		break;
	case 12:
		strcpy(display_string, "Set Second\x0d\x0a");
		displayLCD(1);
		break;
	case 13:
		if (instr == 4) {
			newDate[0]++;
			if (newDate[0] > 99) {
				newDate[0] = 0;
			}
		} else if (instr == 3) {
			if (newDate[0] == 0) {
				newDate[0] = 99;
			} else {
				newDate[0]--;
			}
		}
		sprintf(display_string, "Year: 20%02d\x0d\x0a", newDate[0]);
		displayLCD(1);
		break;

	case 14:
		if (instr == 4) {
			newDate[1]++;
			if (newDate[1] > 12) {
				newDate[1] = 0;
			}
		} else if (instr == 3) {
			if (newDate[1] == 0) {
				newDate[1] = 12;
			} else {
				newDate[1]--;
			}
		}
		sprintf(display_string, "Month: %d\x0d\x0a", newDate[1]);
		displayLCD(1);
		break;
	case 15:
		if (instr == 4) {
			uint8_t daysInMonth[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31,
					30, 31 };
			newDate[2]++;
			if (newDate[2] > daysInMonth[newDate[1] - 1]) {
				newDate[2] = 0;
			}
		} else if (instr == 3) {
			if (newDate[2] == 0) {
				newDate[2] = 12;
			} else {
				newDate[2]--;
			}
		}
		sprintf(display_string, "Day: %d\x0d\x0a", newDate[2]);
		displayLCD(1);
		break;
	case 16:
		if (instr == 4) {
			newDate[3]++;
			if (newDate[3] > 24) {
				newDate[3] = 0;
			}
		} else if (instr == 3) {
			if (newDate[3] == 0) {
				newDate[3] = 24;
			} else {
				newDate[3]--;
			}
		}
		sprintf(display_string, "Hour: %d\x0d\x0a", newDate[3]);
		displayLCD(1);
		break;
	case 17:
		if (instr == 4) {
			newDate[4]++;
			if (newDate[4] > 59) {
				newDate[4] = 0;
			}
		} else if (instr == 3) {
			if (newDate[4] == 0) {
				newDate[4] = 59;
			} else {
				newDate[4]--;
			}
		}
		sprintf(display_string, "Minute: %d\x0d\x0a", newDate[4]);
		displayLCD(1);
		break;
	case 18:
		if (instr == 4) {
			newDate[5]++;
			if (newDate[5] > 59) {
				newDate[5] = 0;
			}
		} else if (instr == 3) {
			if (newDate[5] == 0) {
				newDate[5] = 59;
			} else {
				newDate[5]--;
			}
		}
		sprintf(display_string, "Second: %d\x0d\x0a", newDate[5]);
		displayLCD(1);
		break;
	case 5:
		strcpy(display_string, "Switch state = 0\x0d\x0a");
		displayLCD(1);
		P4_bit.no1 = 0;
		P4_bit.no2 = 0;
		P4_bit.no3 = 0;
		break;
	case 6:
		strcpy(display_string, "Switch state = 1\x0d\x0a");
		displayLCD(1);
		P4_bit.no1 = 0;
		P4_bit.no2 = 0;
		P4_bit.no3 = 1;
		break;
	case 7:
		strcpy(display_string, "Switch state = 2\x0d\x0a");
		displayLCD(1);
		P4_bit.no1 = 1;
		P4_bit.no2 = 0;
		P4_bit.no3 = 0;
		break;
	case 8:
		strcpy(display_string, "Switch state = 3\x0d\x0a");
		displayLCD(1);
		P4_bit.no1 = 0;
		P4_bit.no2 = 1;
		P4_bit.no3 = 0;
		break;
	case 9:
		strcpy(display_string, "Switch state = 4\x0d\x0a");
		displayLCD(1);
		P4_bit.no1 = 1;
		P4_bit.no2 = 1;
		P4_bit.no3 = 0;
		break;
	}
}
