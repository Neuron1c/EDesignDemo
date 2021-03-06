/*
 * lcd.c
 *
 *  Created on: 05 Feb 2016
 *      Author: jtreurn
 */

#include "lcd.h" // or any other includes that makes sense

// Notes:
// This is a basic low-level set of functions to drive the LCD. The functions include
// 	- void initLcd(void) --- set up the LCD; requires a function delayNoInt(time in usec) to operate
// 	- void writeNibbleLcd(uint8_t reg, uint8_t nibble) --- low level command
// 	- void writeByteLcd(uint8_t reg, uint8_t value) --- lowest user level command
//
// To be useful, you should write your own functions to:
//	- write an ASCII character to the LCD
//	- write a whole string to the LCD
//	- write a delay function delayNoInt(delay in usec)

/************************************************************************
* Function Name: writByteLcd()
* Description  : This function writes 2 nibbles to the LCD.
* Arguments    :  reg (BOOL) - 0 = register, 1 = data
*              :  value (BYTE) - first the upper 4 bits and the 4 lower bits
* Actions      : Nibble bus P70-P73 are set in output mode
*              : LCD_RS = 1; LCD_RW -> 0; LCD_E is toggled
* Return Value : None
************************************************************************/

void writeByteLcd(uint8_t reg, uint8_t value)
{
	uint8_t tempReg;

    tempReg = value >> 4;			// Get high nibble
    writeNibbleLcd(reg, tempReg);	// Write high nibble

	tempReg = value & 0x0F;			// Get lower nibble
    writeNibbleLcd(reg, tempReg);	// Write lower nibble
}

/************************************************************************
* Function Name: writeNibbleLcd()
* Description  : This function writes a nibble to the LCD.
* Arguments    : value (BYTE) - only the lower 4 bits are used
* Actions      : Nibble bus P70-P73 set in output mode
*              : LCD_RS = 1; LCD_RW -> 0; LCD_E is toggled
* Return Value : None
************************************************************************/

void writeNibbleLcd(uint8_t reg, uint8_t nibble)
{
	uint8_t tempReg;

    if (reg)
        LCD_RS = 1U;							// Read/write RAM
    else
        LCD_RS = 0U;							// Command

    LCD_RW   = 0U;								// Set to Write mode
    PM7     &= 0xF0;        					// Set Port 7 Data nibble to output mode
    tempReg  = (P7 & 0x0F) | (nibble << 4);  	// Combine nibble and P7
    P7       = tempReg;     					// Load Port 7 data nibble
    SET_LCD_EN_HIGH;							// LCD_E  = 1U; (Enable high)
        // Wait > 0.3 usec
    NOP(); NOP(); NOP(); NOP(); NOP(); NOP();

    SET_LCD_EN_LOW;								// LCD_E  = 0U; (Set Enable low)
    NOP(); NOP();   							// Wait (2 x T_CLK)
//    PM7 |= 0x0F;    							// Set Port 7 Data to input mode
    LCD_RW = 1U;    							// Set to Read mode
}


/************************************************************************
* Function Name: initLcd()
* Description  : This function initializes the LCD.
* Arguments    :  none
* Actions      : Provide all the init instructions - clear screen
* Return Value : None
************************************************************************/

void initLcd(void)
{
	TMMK00 = 1U;					// Do not use interrupts here, just wait for the flags

	delayNoInt(15000);				// wait 15 msec for LCD power up

    writeNibbleLcd(LCD_CTRL_WR, 0x3); 	// Command 1 -> 8 bits
    delayNoInt(4100);				// Start timer with 4100 usec delay

    writeNibbleLcd(LCD_CTRL_WR, 0x3);	// Command 2 -> 8 bits
    delayNoInt(100);				// Start timer with 100 usec delay

    writeNibbleLcd(LCD_CTRL_WR, 0x3);	// Command 3 -> 8 bits
    delayNoInt(40);					// Start timer with 40 usec delay

    writeNibbleLcd(LCD_CTRL_WR, 0x2);	// Command 4 -> 4 bits
    delayNoInt(40);					// Start timer with 40 usec delay

    writeByteLcd(LCD_CTRL_WR, FUNCTION_SET);  // Set 2 lines, 5x7 dots
	delayNoInt(40);					// Start timer with 40 usec delay

	writeByteLcd(LCD_CTRL_WR, LCD_CURSOR_ON);     // Display Cursor ON
	delayNoInt(40);					// Start timer with 40 usec delay

	writeByteLcd(LCD_CTRL_WR, LCD_CLEAR);          // Clear Display
	delayNoInt(1640);				// Start timer with 1640 usec delay

	writeByteLcd(LCD_CTRL_WR, CURSOR_MODE_INC);    // Entry Mode set
	delayNoInt(40);					// Start timer with 40 usec delay

	writeByteLcd(LCD_CTRL_WR, LCD_HOME_L1);
	delayNoInt(40);					// Start timer with 40 usec delay

}

void startTMR0(int delay){
	TMIF00 = 0U; // Clear any interrupt flag
	TDR00 = delay; // Load register (in 1 usec units)
	TS0 = 1U; // Start timer
}
void delayNoInt(uint16_t delay){
	startTMR0(delay); // Start timer with delay value
		while(TMIF00 == 0) // Wait for timer flag
			NOP();
		    R_TAU0_Channel0_Stop(); // Stop timer and clear all flags
}


