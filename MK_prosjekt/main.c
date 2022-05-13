/*
* MK_prosjekt.c
*
* Created: 10.04.2022 12:04:14
* Author : Idar Evenstad
*/

#include "SystemRTCandClk.h"

char TxBuffer[24];
int main(void){
	System_init();
	while (1)
	{
		UART_MENU();
		
	}
}

ISR(RTC_CNT_vect){
	RTC.INTFLAGS = RTC_OVF_bm;
	ISR_TWI();
}

ISR(USART3_RXC_vect){
	UART_ISR();
}
