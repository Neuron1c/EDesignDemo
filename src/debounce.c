/*
 * debounce.c
 *
 *  Created on: 12 Apr 2017
 *      Author: 19119038
 */

#include "r_cg_userdefine.h"
#include "debounce.h"

//static uint16_t high[pin_no];
//static uint16_t low[pin_no];
//static uint16_t high0,high1,high2,high3,high4,high5,high6,high7;
//static uint16_t low0,low1,low2,low3,low3,low4,low5,low6,low7;
//static uint8_t flag0,flag1,flag2,flag3,flag4,flag5,flag6,flag7;
//uint8_t flag[pin_no];

static uint16_t low[8];
static uint16_t high[8];
static uint16_t flag[8];

uint8_t bounce_correction(volatile __BITS8 *port, int pin_no){


if(getPin(port, pin_no) == 1){
	high[pin_no]++;
	low[pin_no] = 0;
}
else{
	low[pin_no]++;
	high[pin_no] = 0;
}

if(high[pin_no] > 1250){
	high[pin_no] = 1251;
	if(flag[pin_no] == 0){
		flag[pin_no] = 1;
		return 1;
	}
}
else if(low[pin_no] > 1250){
		flag[pin_no] = 0;
		low[pin_no] = 0;
}

return 0;

}

unsigned char getPin(volatile __BITS8 *port, int pin_no){

	switch(pin_no){
		case 0: return port->no0; break;
		case 1: return port->no1; break;
		case 2: return port->no2; break;
		case 3: return port->no3; break;
		case 4: return port->no4; break;
		case 5: return port->no5; break;
		case 6: return port->no6; break;
		case 7: return port->no7; break;
		default: return -1;

	}

	return 0;
}

