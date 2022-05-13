/*
 * AVR_Analog_RW.h
 *
 * Created: 10.04.2022 12:12:10
 *  Author: Idar Evenstad
 */ 


#ifndef AVR_Analog_RW_H_
#define AVR_Analog_RW_H_
#include "SystemRTCandClk.h"

//Analog write ports
#define PD0 PIN0_bm 
#define PD1 PIN1_bm 
#define PD2 PIN2_bm 
#define PD3 PIN3_bm 
#define PB2 PIN1_bm 
#define PB3 PIN2_bm 
#define PB4 PIN4_bm 
#define PB5 PIN5_bm 

//prototypes
uint16_t AnalogRead(char port);
void ADC_Init();

void AnalogWrite(char ch,char duty);
void TCA0_init();
void TCA1_init();
void PWM_init();

uint8_t dutycalc(uint8_t pre);

void TCB0_init();

uint32_t Read_Tacho(char ch);





#endif /* AVR_Analog_RW_H_ */


