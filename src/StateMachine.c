/*
 * StateMachine.c
 *
 *  Created on: 15 Apr 2017
 *      Author: Neuron1c
 */

#include "r_cg_userdefine.h"

static int state = 0;
extern uint8_t mem[RX_BUF_LEN];

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
		case 0: break;
		case 1: break;
		case 2: break;
		case 3: break;
		case 4: break;
		case 5: break;
		case 6: break;
		case 7: break;
		case 8: break;
		case 9: break;
	}
}
