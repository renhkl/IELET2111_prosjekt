/*
 * IncFile1.h
 *
 * Created: 12.05.2022 23:01:17
 *  Author: Idar Evenstad
 */ 


#ifndef SystemRTCandClk_H_
#define SystemRTCandClk_H_

#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include <util/delay.h>
#include <avr/eeprom.h>
#include <string.h>
#include "UARTplus.h"
#include "AVR_Analog_RW.h"
#include "TWI.h"
#include "EEPROM.h"

void RTC_init(int periode);
void Osc_init(char main_clk);
void System_init();



#endif /* INCFILE1_H_ */