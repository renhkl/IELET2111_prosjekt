/*
 * AVR_Analog_RW.h
 *
 * Created: 10.04.2022 12:12:10
 *  Author: Idar Evenstad
 */ 


#ifndef AVR_Analog_RW_H_
#define AVR_Analog_RW_H_





#endif /* AVR_Analog_RW_H_ */
#include <avr/io.h>


//Analog write ports
#define AnalogPinW0 PIN2_bm //PA2
#define AnalogPinW1 PIN3_bm //PA3
#define AnalogPinW2 PIN4_bm //PA4
#define AnalogPinW3 PIN5_bm //PA5
#define AnalogPinW4 PIN1_bm //PB2
#define AnalogPinW5 PIN2_bm //PB3
#define AnalogPinW6 PIN4_bm //PB4
#define AnalogPinW7 PIN5_bm //PB5

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

