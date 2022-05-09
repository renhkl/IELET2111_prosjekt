/*
 * myEEPROM.h
 *
 * Created: 09.05.2022 11:50:06
 *  Author: Mathias Jørgensen
 */ 

#include <stdio.h>
#include <avr/io.h>
#include <avr/cpufunc.h>
#include <avr/eeprom.h>
#define F_CPU 16000000UL
#include "util/delay.h"

#define fan_0_EPPROM_adr 0
#define fan_1_EPPROM_adr 64
#define fan_2_EPPROM_adr 128
#define fan_3_EPPROM_adr 192
#define fan_4_EPPROM_adr 256
#define fan_5_EPPROM_adr 320
#define fan_6_EPPROM_adr 384
#define fan_7_EPPROM_adr 448

// Prototypes
void my_eeprom_update_byte(uint8_t adr, uint8_t value);
void my_eeprom_update_char(uint8_t adr, uint16_t value);
uint8_t my_eeprom_read_byte(uint8_t adr);
uint16_t my_eeprom_read_char(uint8_t adr);