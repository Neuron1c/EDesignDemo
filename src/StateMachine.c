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

//Recieve instruction, value from 1 to 7
//1 = right button (accept/okay)
//2 = middle button
//3 = down button
//4 = up button
//5 = left button (back/cancel)
//6 = switch to test mode
//7 = switch to normal mode

void recieveInstruction(int instr){
	switch(state){

	//---------------------------------------------------- normal mode

	case 0: if(instr == 3)state++;
			else if(instr == 4)state = 4;
			else if(instr == 6)state = 5; break;
	case 1: if(instr == 3)state++;
			else if(instr == 4)state--;
			else if(instr == 6)state = 5; break;
	case 2: if(instr == 3)state++;
			else if(instr == 4)state--;
			else if(instr == 6)state = 5;break;
	case 3: if(instr == 3)state++;
			else if(instr == 4)state--;
			else if(instr == 6)state = 5; break;
	case 4: if(instr == 3)state = 0;
			else if(instr == 4)state--;
			else if(instr == 6)state = 5; break;

	//---------------------------------------------------- test mode

	case 5: if(instr == 3)state++;
			else if(instr == 4)state = 9;
			else if(instr == 7)state = 0; break;
	case 6: if(instr == 3)state++;
			else if(instr == 4)state--;
			else if(instr == 7)state = 0; break;
	case 7: if(instr == 3)state++;
		    else if(instr == 4)state--;
		    else if(instr == 7)state = 0;  break;
	case 8: if(instr == 3)state++;
			else if(instr == 4)state--;
			else if(instr == 7)state = 0; break;
	case 9: if(instr == 3)state = 5;
			else if(instr == 4)state--;
			else if(instr == 7)state = 0; break;
	}

	switch(state){
		case 0: strcpy(display_string,"0\x0d\x0a"); displayLCD(1); break;
		case 1: strcpy(display_string,"1\x0d\x0a"); displayLCD(1); break;
		case 2: strcpy(display_string,"2\x0d\x0a"); displayLCD(1); break;
		case 3: strcpy(display_string,"3\x0d\x0a"); displayLCD(1); break;
		case 4: strcpy(display_string,"4\x0d\x0a"); displayLCD(1); break;
		case 5: strcpy(display_string,"5\x0d\x0a"); displayLCD(1); break;
		case 6: strcpy(display_string,"6\x0d\x0a"); displayLCD(1); break;
		case 7: strcpy(display_string,"7\x0d\x0a"); displayLCD(1); break;
		case 8: strcpy(display_string,"8\x0d\x0a"); displayLCD(1); break;
		case 9: strcpy(display_string,"9\x0d\x0a"); displayLCD(1); break;
	}
}
