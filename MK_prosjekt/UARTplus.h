/*
 * UARTplus.h
 *
 * Created: 07.05.2022 14:53:44
 *  Author: Idar Evenstad
 */ 


#ifndef UARTplus_H_
#define UARTplus_H_





#endif /* INCFILE1_H_ */

#include <avr/io.h>
#define F_CPU 4000000UL
#include <util/delay.h>
#include <stdio.h>
#include <string.h>

#include "AVR_Analog_RW.h"
#include "myEEPROM.h"

#define UART_bps 9600L //Define the UART bit per second, up to 57600

#define IsBufferEmpty (RingbufferHead == RingbufferTail) //Check if buffer is empty      //DELETE MAYBE

#define TxBufferFlag (USART3_STATUS & (1 << USART_DREIF_bp)) //Flag for indicating empty register and ready to receive new data

//Registers to send data to Tx or Rx
#define UART_TX_BUFFER USART3.TXDATAL //Tx 8-bit register
#define UART_RX_BUFFER USART3.RXDATAL //Rx 8-bit register

//Special ASCII-characters
#define Bell 0x07		 //Alert end of line
#define backspace 0x08	 //Backspace
#define backspace127 127 //Sometimes backspace is ASCII 0x7F or 127 for some terminals
#define CR 0x0D			 //Carriage Return, control character to return to the beginning of the line | often used with LF for <enter>
#define LF 0x0A			 //Line Feed, control character to advance the cursor to the next line

//UART ports
#define TxD 0 //PB0
#define RxD 1 //PB1
#define XCK 2 //PB2
#define XDIR 3 //PB3

//Buffersizes
#define TxBufferLength 48
#define RxBufferLength 24
#define MaxWordLength 12

//Command sizes
#define NoOfCommands 8 //No of commands in commandArray
#define MaxCommandLength 13 //Max length of every command

//BOOL values
#define FALSE 0
#define TRUE 1

//Prototypes
void Osc_init(void);
void UART3_init(void);

//General UART functions
void UART_Echo(char recvbyte );
void ResetSHell(void);
void UART_EchoCheck(void);

//Some UART write prototypes
void UART_SendChar(char CharToSend);
void UART_SendBuffer(void);
void UART_NewLine(void);
void CharInBUF(char incominchar);
void UART_TitleAll(void);
void UART_Row(char row,char numofsamples);
void UART_ReadChannel(char ch);
void UART_ReadAll(void);

//Returning UART prototypes
void Ringbuffer_Reset(void);
char UART_getRxBufferChar(void);
char ProcessWord(void);
char SearchForCommand(void);
int UART_CheckFanInput(char digits);
int UART_DigitsCheck(void);
int UART_FanSpeedSet(void);

//ALL fans on/off
void AllON(void);
void AllOFF(void);

int Tacho_filter(char ch);
int NumCheck(int num);


void ISR_Func(void);
void UART_MENU(void);

void Buffer_init(void);

void TxBuffer_StringWrite(char* buf, char* str);
void TxBuffer_IntWrite(char* buf, int data);

void EchoToggle(void);
