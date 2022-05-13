/*
 * myEEPROM.h
 *
 * Created: 09.05.2022 11:50:06
 *  Author: Mathias Jørgensen
 */ 

#ifndef EEPROM_H_
#define EEPROM_H_

#include "SystemRTCandClk.h"


//EEPROM addresses
#define fan_0_EEPROM_adr 0
#define fan_1_EEPROM_adr 2
#define fan_2_EEPROM_adr 4
#define fan_3_EEPROM_adr 6
#define fan_4_EEPROM_adr 8
#define fan_5_EEPROM_adr 10
#define fan_6_EEPROM_adr 12
#define fan_7_EEPROM_adr 14

// Prototypes
void my_eeprom_update_byte(uint8_t adr, uint8_t value);
void my_eeprom_update_char(uint8_t adr, uint16_t value);
uint8_t my_eeprom_read_byte(uint8_t adr);
uint16_t my_eeprom_read_char(uint8_t adr);
#endif /* INCFILE1_H_ */