/*
 * CFile1.c
 *
 * Created: 12.05.2022 22:51:09
 *  Author: Idar Evenstad
 */ 

#include "TWI.h"

//Status data for slave
volatile uint8_t status = 0;

//Read and dump values -> not relevant
volatile uint8_t humidbyteH = 0;
volatile uint8_t humidbyteL = 0;

//20-bit tempbytes
volatile uint8_t humidtempbyte = 0; //Last half is temp values 4 bit [1. values]
volatile uint8_t tempbyteH = 0; //8 bit temp values [2. values]
volatile uint8_t tempbyteL = 0; //8 bit temp values [3. values]

//Temperature
float temperature = 0;

//Buffer for writing UART
char TxBuffer[24];

void TWI_getTemp(void){
	TWI_StartTemp();
	TWI_TransmittAddr(AHT10_ADDRESS, READ);
	//Save array in individual values from array
	uint8_t *arr = TWI_ReadTempdata();
	humidtempbyte = arr[0];
	tempbyteH = arr[1];
	tempbyteL = arr[2];
	temperature = TempCalc();
	TempToUART(temperature);
	_delay_ms(1000);
	return temperature;
}
void TWI_EndRead(void){
	while(!I2C_DATA_RECEIVED);
	TWI0.MCTRLB = TWI_MCMD_STOP_gc | TWI_ACKACT_NACK_gc; //Sends stop and NACK to end read operation
}
uint8_t* TWI_ReadTempdata(void){
	uint8_t *tempdata = (uint8_t *) malloc(sizeof(uint8_t) * 3);
	while(!I2C_DATA_RECEIVED);	//Wait until data is received
	status = TWI0.MDATA;		//Status data for slave
	while(!I2C_DATA_RECEIVED);
	humidbyteH = TWI0.MDATA;	//Read data
	while(!I2C_DATA_RECEIVED);
	humidbyteL = TWI0.MDATA;
	while(!I2C_DATA_RECEIVED);
	humidtempbyte = TWI0.MDATA;
	*tempdata = humidtempbyte;	//Save in array
	while(!I2C_DATA_RECEIVED);
	tempbyteH = TWI0.MDATA;
	*(tempdata+1) = tempbyteH;
	while(!I2C_DATA_RECEIVED);
	tempbyteL = TWI0.MDATA;
	*(tempdata+2) = tempbyteL;
	TWI_EndRead();
	return tempdata;			//return array
}
void TWI_StartTemp(void){
	TWI_TransmittAddr(AHT10_ADDRESS, WRITE);
	TWI_TransmittData(0xAC);				     //Command for triggering measurement
	TWI_TransmittData(0x03);					 //DATA0, unspecified but mentioned in AHT10 datasheet
	TWI_TransmittData(0x00);				     //DATA1, unspecified but mentioned in AHT10 datasheet
	TWI_EndWrite();                              //Send operation is over
	_delay_ms(75);								 //delay as said in the AHT10 datasheet
}
void TWI_TransmittData(char data){
	while(I2C_SLAVE_NOT_ACK | I2C_NOT_COMPLETE); //Check if slave ACK is received
	TWI0.MDATA = data;
}
void TWI_EndWrite(void){
	while(I2C_SLAVE_NOT_ACK | I2C_NOT_COMPLETE);
	TWI0.MCTRLB = TWI_MCMD_STOP_gc;
}

void TWI_TransmittAddr(char addr, char RWbit){
	TWI0.MADDR = TWI_SET_ADDR(addr, RWbit);
}

void TWI_init(void){
	TWI0.MBAUD = (uint8_t)TWI0_BAUD(I2C_SCL_FREQ, 0);			//Set the Baudrate for
	TWI0.MCTRLA = TWI_ENABLE_bm;								//Enable TWI
	TWI0.MSTATUS = TWI_BUSSTATE_IDLE_gc;						//Set bus to IDLE
	TWI0.MCTRLA |= TWI_SMEN_bm;									//Set to SmartMode to auto ACK after receivin data
}

float TempCalc(void){
	//Temporary values
	uint16_t tempbyte = 0;
	uint32_t temporarybyte = 0;
	
	temporarybyte = humidtempbyte & 0b00001111; //Remove humidity data
	temporarybyte = temporarybyte << 16; //Shift to get 20-bit
	tempbyte = tempbyteH << 8 | tempbyteL; //Merge togheter to get 16-bit
	temporarybyte = temporarybyte | tempbyte; //Or togheter to get the full 20-bit
	
	temperature = ((temporarybyte/(float)1048576)*200-50); //Calculation for temperature, from AHT10 datasheet
	return temperature;
}

void TempToUART(float temp){
	sprintf(TxBuffer, "temp = %.2f", temp);
	UART_SendBuffer();
	UART_NewLine();
}

void ISR_TWI(){
		TWI_getTemp();
		EEPROM_SaveFanStatus();
		if(temperature > 70){
			TxBuffer_StringWrite("Check Temp");
			UART_NewLine();
		}
		else if(temperature > 85){
			TxBuffer_StringWrite("Temp to High, shutting down");
			UART_NewLine();
			AllOFF();
			} else {
			TxBuffer_StringWrite("Ok");
			UART_NewLine();
		}
}