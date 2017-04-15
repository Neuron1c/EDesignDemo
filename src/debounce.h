/*
 * debounce.h
 *
 *  Created on: 12 Apr 2017
 *      Author: 19119038
 */

#ifndef DEBOUNCE_H_
#define DEBOUNCE_H_

uint8_t bounce_correction(volatile __BITS8 *port, int pin_no);
unsigned char getPin(volatile __BITS8 *port, int pin_no);

#endif /* DEBOUNCE_H_ */
