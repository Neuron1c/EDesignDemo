/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products.
* No other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws. 
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIESREGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED
* OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NON-INFRINGEMENT.  ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY
* LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE FOR ANY DIRECT,
* INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR
* ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability 
* of this software. By using this software, you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2011, 2015 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/

/***********************************************************************************************************************
* File Name    : r_main.c
* Version      : CodeGenerator for RL78/G13 V2.03.02.01 [15 May 2015]
* Device(s)    : R5F100LE
* Tool-Chain   : GCCRL78
* Description  : This file implements main function.
* Creation Date: 2017/04/12
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_cgc.h"
#include "r_cg_port.h"
#include "r_cg_serial.h"
#include "r_cg_adc.h"
#include "r_cg_timer.h"
#include "r_cg_rtc.h"
#include "r_cg_it.h"
/* Start user code for include. Do not edit comment generated here */
#include <string.h>
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
uint8_t uart1RxBuf[RX_BUF_LEN];
uint16_t uart1RxCnt;
uint8_t uart1RxFlag;
uint8_t uart1TxBuf[TX_BUF_LEN];
uint16_t uart1TxCnt;
uint8_t uart1TxFlag;
uint8_t	uart1RxErrFlag;	// UART2 Receive Error Flag
uint8_t uart1RxOvrFlag;	// UART2 Receive Overrun Flag
MD_STATUS uart1Status;
//extern uint8_t mem[RX_BUF_LEN];
/* End user code. Do not edit comment generated here */
void R_MAIN_UserInit(void);

/***********************************************************************************************************************
* Function Name: main
* Description  : This function implements main function.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void main(void)
{

	R_MAIN_UserInit();
    /* Start user code. Do not edit comment generated here */

    R_UART1_Create();
    R_UART1_Start();

    R_IT_Create();
//  R_IT_Start();

    R_RTC_Create();
    R_RTC_Start();

    initLcd();

    char welcome[] = "Welcome\x0d\x0a";
    R_UART1_Send(welcome,9);

    uart1Status = R_UART1_Receive(&uart1RxBuf[0],1);

    while (1U)
    {
		 R_ADC_Set_OperationOn();

		 while(ADCS);

		 if(bounce_correction(&P5_bit, 5)){
			 recieveInstruction(5);
		 }
		 if(bounce_correction(&P5_bit, 4)){
			 recieveInstruction(3);
		 }
		 if(bounce_correction(&P5_bit, 3)){
			 recieveInstruction(4);
		 }
		 if(bounce_correction(&P5_bit, 2)){
			 recieveInstruction(1);
		 }

            if (uart1RxFlag)
            {
            	uart1RxFlag = 0U;

            	populate(uart1RxBuf[0]);

                uart1Status = R_UART1_Receive(uart1RxBuf,1);
            }

    }
    /* End user code. Do not edit comment generated here */
}


/***********************************************************************************************************************
* Function Name: R_MAIN_UserInit
* Description  : This function adds user code before implementing main function.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_MAIN_UserInit(void)
{
    /* Start user code. Do not edit comment generated here */
    EI();
    /* End user code. Do not edit comment generated here */
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
