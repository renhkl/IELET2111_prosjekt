/*
 * IncFile1.h
 *
 * Created: 12.05.2022 22:51:16
 *  Author: Idar Evenstad
 */ 


#ifndef TWI_H_
#define TWI_H
#include "SystemRTCandClk.h"

//Clock Frequency for SCL
#define I2C_SCL_FREQ 100000UL //standard mode Frequency

//Baud calculation
#define TWI0_BAUD(F_SCL, T_RISE)  (((float)F_CPU/(2*(float)F_SCL)) - (5 + ( ((float)F_CPU * (float)T_RISE) /2)))

//Slave-address
#define AHT10_ADDRESS 0x38

//Address Direction
#define READ  1
#define WRITE  0

//#define TWI_SET_ADDR
#define TWI_SET_ADDR(address, RWbit) (address << 1) | (RWbit << 0)

//Flag checks
#define I2C_SLAVE_NOT_ACK (TWI_RXACK_bm & TWI0_MSTATUS)
#define I2C_DATA_RECEIVED (TWI_RIF_bm & TWI0_MSTATUS)
#define I2C_NOT_COMPLETE (!(TWI_WIF_bm & TWI0_MSTATUS))

//TWI functions
void TWI_init(void);

//Returning functions
uint8_t* TWI_ReadTempdata();
float TempCalc(void);
void TempToUART(float temp);

void TWI_TransmittAddr(char addr, char RWbit);
void TWI_StartTemp(void);
void TWI_TransmittData(char data);
void TWI_EndWrite(void);
void TWI_EndRead(void);
void TWI_getTemp(void);

void ISR_TWI();
#endif /* INCFILE1_H_ */