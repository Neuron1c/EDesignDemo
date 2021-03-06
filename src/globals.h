/*
 * globals.h
 *
 *  Created on: 15 Feb 2017
 *      Author: 19119038
 */

#ifndef GLOBALS_H_
#define GLOBALS_H_

#define	RX_BUF_LEN	50
#define	TX_BUF_LEN	50

#include "r_cg_userdefine.h"
#include "r_cg_macrodriver.h"

uint8_t record[RX_BUF_LEN];
uint8_t mem[RX_BUF_LEN];
uint8_t display_string[RX_BUF_LEN];
uint8_t voltage, current1, current2;
uint8_t date[6];
uint8_t scrollFlag;
uint8_t tempFlag;
uint8_t secFlag;
uint16_t scrollCount;
int timerFlag;

#endif /* GLOBALS_H_ */
