/*
 * PWM.c
 *
 * Created: 12.05.2022 16:13:32
 * Author : Mathias Jørgensen
 */ 

#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/cpufunc.h>
#include <avr/eeprom.h>
#define F_CPU 16000000UL
#include "util/delay.h"

#define USART3_BAUD_RATE(BAUD_RATE) ((uint16_t)(F_CPU * 64 / (16 * BAUD_RATE)))

// PROTOTYPES
void systemInit(void);
void USART3_sendChar(char c);
void USART3_sendString(const char *str);
void USART3_printChar(char c, FILE *stream);
FILE USART_stream = FDEV_SETUP_STREAM(USART3_printChar, NULL, _FDEV_SETUP_WRITE);

void TCA0_init(void);
void Port_init(void);
uint16_t dutycalc(uint8_t pre);

int main(void)
{
	systemInit();
	TCA0_init();
	Port_init();
	
	int duty = 10;
	int return_duty;
	
	return_duty = dutycalc(duty);
	
	printf("start\n\r");
	
	sei();
	
    while (1) 
    {	
		// trengs egt ikke kjøres i while, trur æ
		TCA0.SINGLE.CMP0 = return_duty;	
		_delay_ms(1000);
    }
}


// FUNCTION
void systemInit(void)
{
	ccp_write_io((uint8_t *) &CLKCTRL.XOSCHFCTRLA, CLKCTRL_RUNSTDBY_bm
	| CLKCTRL_CSUTHF_4K_gc
	| CLKCTRL_FRQRANGE_16M_gc
	| CLKCTRL_SELHF_CRYSTAL_gc
	| CLKCTRL_ENABLE_bm);
	while(!(CLKCTRL.MCLKSTATUS & CLKCTRL_EXTS_bm)); // Wait for Xtal startup
	// Next, set the main clock to use XOSCHF as source, and enable the CLKOUT pin
	ccp_write_io((uint8_t *) &CLKCTRL.MCLKCTRLA, CLKCTRL_CLKSEL_EXTCLK_gc | CLKCTRL_CLKOUT_bm);
	
	USART3.BAUD = USART3_BAUD_RATE(57600); // set baud rate
	USART3.CTRLB |= USART_TXEN_bm;  // enable transmitter
	PORTMUX.USARTROUTEA |= 0x00; //PORTMUX_USART00_bm; // change this to use alternate UART output
	PORTB.DIR |= PIN0_bm; // B0 is the USART3 USART TX output pin on Curiosity Nano
	stdout = &USART_stream;
}

void USART3_sendChar(char c) {
	while (!(USART3.STATUS & USART_DREIF_bm)); // wait for last char to be completed
	USART3.TXDATAL = c; // put the char to the uart TX
}

void USART3_printChar(char c, FILE *stream)
{
	USART3_sendChar(c);
}

void USART3_sendString(const char *str)
{
	uint16_t i = 0;
	while( str[i] != '\0')
	{
		USART3_sendChar(str[i++]);
	}
}

uint16_t dutycalc(uint8_t pre){
	uint16_t duty = 0xFFFF*(pre*0.01);
	return duty;
}


// DT som e verdt å se på for PWM signal

void TCA0_init(void)
{
	PORTMUX.ACROUTEA = PORTMUX_TCA0_PORTC_gc;
	TCA0.SINGLE.PER = 0xFFFF;
	TCA0.SINGLE.CTRLB = TCA_SINGLE_CMP0_bm | TCA_SINGLE_CMP1_bm | TCA_SINGLE_CMP2_bm | 0x03; // 0x03 setter singel-slope pwm
	TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV1024_gc | TCA_SINGLE_ENABLE_bm;
	TCA0.SINGLE.INTCTRL = TCA_SINGLE_CMP0_bm | TCA_SINGLE_OVF_bm;
}

void Port_init(void)
{
	PORTC.DIR = 0xFF;
}

ISR(TCA0_CMP0_vect)
{
	PORTC.OUT ^= PIN0_bm; // eventuelt outset = 0xFF
	printf("CMP \n\r");
	
	TCA0.SINGLE.INTFLAGS = TCA_SINGLE_CMP0_bm;
}

ISR(TCA0_OVF_vect)
{
	PORTC.OUT ^= PIN0_bm; // og outset = 0x00 så e det ikke fare for at det endres underveis i programmet 
	printf("OVF \n\r");
	
	TCA0.SINGLE.INTFLAGS = TCA_SINGLE_OVF_bm;
}