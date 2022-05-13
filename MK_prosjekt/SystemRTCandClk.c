/*
 * CFile1.c
 *
 * Created: 12.05.2022 23:01:06
 *  Author: Idar Evenstad
 */ 
#include "SystemRTCandClk.h"

void RTC_init(int periode){
	_PROTECTED_WRITE(CLKCTRL.XOSC32KCTRLA, CLKCTRL_ENABLE_bm);
	RTC.CLKSEL = RTC_CLKSEL_XOSC32K_gc;
	RTC.PER = periode; 
	RTC.INTCTRL = RTC_OVF_bm;
	RTC.CTRLA = RTC_PRESCALER_DIV32768_gc | RTC_RTCEN_bm;
}

//Set internal oscillator to set clock
void Osc_init(char main_clk){
	//_PROTECTED_WRITE() allows writing to protected registers CCP = 0xD8 | IOREG
	_PROTECTED_WRITE(CLKCTRL_OSCHFCTRLA,(main_clk << CLKCTRL_FREQSEL0_bp)); 
}

void System_init(){
	PWM_init(); //PWM write init | Initialization of TCA clock
	Osc_init(7); //Initialize Internal 4MHz Oscillator clock
	UART3_init(UART_bps); //Initialize UART
	TCB0_init(); //Initialize TCB0 for measuring frequency
	TWI_init();
	RTC_init(0xE10); //1 hour periode
	sei(); //Global interrupt enable
}