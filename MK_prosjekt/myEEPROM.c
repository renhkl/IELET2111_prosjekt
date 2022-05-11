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
	while (NVMCTRL.STATUS & NVMCTRL_EEBUSY_bm); //Holds the program until the NVMCTRL register is ready to receive data
	_PROTECTED_WRITE_SPM(NVMCTRL.CTRLA, 0x13);	//Sets register to Erase and Write (EEERWR) | and sets the CPU.CCP register to SPM or Allow Self-Programming
	*(uint8_t*) (MAPPED_EEPROM_START+adr) = value;	//Writes data to EEPROM
	_PROTECTED_WRITE_SPM(NVMCTRL.CTRLA, 0x00); //Sets the register back to no command (NOCMD)
}

void my_eeprom_update_char(uint8_t adr, uint16_t value)
{
	uint8_t valueHigh;
	uint8_t valueLow;
	
	//Converts from uint8 to uint16
	valueHigh = (value >> 8);
	valueLow = value & 0xff;
	
	my_eeprom_update_byte(adr, valueHigh); //Sets the HIGH byte
	my_eeprom_update_byte((adr+1), valueLow); //Sets the LOW byte
}


// Read from EEPROM
uint8_t my_eeprom_read_byte(uint8_t adr)
{
	uint8_t byte;
	byte = *(uint8_t*) (MAPPED_EEPROM_START+adr); // Reads value from EEPROM
	return byte; //Returns the read value
}

uint16_t my_eeprom_read_char(uint8_t adr)
{
	uint8_t valueHigh;
	uint8_t valueLow;
	
	valueHigh = my_eeprom_read_byte(adr); //Reads the High-byte
	valueLow = my_eeprom_read_byte(adr+1); //Reads the Low-byte
	
	uint16_t value;
	value = ((valueHigh << 8) | valueLow);	//Adds the values together to get a complete uint16
	
	return value; //Returns the read value
}
