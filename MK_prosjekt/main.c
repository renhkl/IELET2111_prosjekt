/*
* MK_prosjekt.c
*
* Created: 10.04.2022 12:04:14
* Author : Idar Evenstad
*/

#include <avr/interrupt.h>
#include "UARTplus.h"
#include "AVR_Analog_RW.h"


//char TxBuffer[24];
int main(void){
	Buffer_init(); //Sets the buffer for UART Menu
	//PWM_init(); //PWM write init | Initialization of TCA clock
	Osc_init(); //Initialize Internal 16MHz Oscillator clock
	UART3_init(); //Initialize UART
	TCB0_init(); //Initialize TCB0 for measuring frequency
	
	sei(); //Global interrupt enable
	while (1)
	{
		UART_MENU();
		
	}

}

//Interrupt for the rx
ISR(USART3_RXC_vect){
	ISR_Func();
}
