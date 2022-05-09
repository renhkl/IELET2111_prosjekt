/*
 * myEEPROM.c
 *
 * Created: 09.05.2022 11:50:28
 *  Author: Mathias Jørgensen
 */ 

#include "myEEPROM.h"

// Write
void my_eeprom_update_byte(uint8_t adr, uint8_t value)
{
	while (NVMCTRL.STATUS & NVMCTRL_EEBUSY_bm); // Sjekker for error i NVMCTRL registeret og venter til de eventullet er ferdig
	_PROTECTED_WRITE_SPM(NVMCTRL.CTRLA, 0x13);	// Skrur på slett og skriv (EEERWR)
	*(uint8_t*) (MAPPED_EEPROM_START+adr) = value;	// Skriver verdien inn til EEPROM
	_PROTECTED_WRITE_SPM(NVMCTRL.CTRLA, 0x00); // Avlutter skriving og andre kommandoer(NOCMD)
}

void my_eeprom_update_char(uint8_t adr, uint16_t value)
{
	uint8_t valueHigh;
	uint8_t valueLow;
	
	// Gjør verdien om til to uint8_t i stedet for en uint16_t
	valueHigh = (value >> 8);
	valueLow = value & 0xff;
	
	my_eeprom_update_byte(adr, valueHigh);
	my_eeprom_update_byte((adr+1), valueLow);
}


// Read
uint8_t my_eeprom_read_byte(uint8_t adr)
{
	uint8_t byte;
	byte = *(uint8_t*) (MAPPED_EEPROM_START+adr); // Leser av verdien fra EEPROM
	return byte;
}

uint16_t my_eeprom_read_char(uint8_t adr)
{
	uint8_t valueHigh;
	uint8_t valueLow;
	
	valueHigh = my_eeprom_read_byte(adr);
	valueLow = my_eeprom_read_byte(adr+1);
	
	uint16_t value;
	value = ((valueHigh << 8) | valueLow);	// Legger verdiene sammen til en uint16_t i stedet for to uint8_t
	
	return value;
}