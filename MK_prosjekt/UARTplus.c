/*
* UARTplus.c
*
* Created: 07.05.2022 14:53:35
*  Author: Idar Evenstad
*/

#include "UARTplus.h"


//Speed variables | For calculating and usage
int speed = 0;
int n1, n2 = 0;

int speedch0 = 0;
int speedch1 = 0;
int speedch2 = 0;
int speedch3 = 0;
int speedch4 = 0;
int speedch5 = 0;
int speedch6 = 0;
int speedch7 = 0;


//Speed variables | For trend data
uint16_t SpeedFromEEPROMch[8] = {0, 0, 0, 0, 0, 0, 0, 0};
uint16_t SpeedToEEPROMch[8] = {0, 0, 0, 0, 0, 0, 0, 0};

/*
uint16_t SpeedToEEPROMch0 = 0;
uint16_t SpeedToEEPROMch1 = 0;
uint16_t SpeedToEEPROMch2 = 0;
uint16_t SpeedToEEPROMch3 = 0;
uint16_t SpeedToEEPROMch4 = 0;
uint16_t SpeedToEEPROMch5 = 0;
uint16_t SpeedToEEPROMch6 = 0;
uint16_t SpeedToEEPROMch7 = 0;

uint16_t SpeedFromEEPROMch0 = 0;
uint16_t SpeedFromEEPROMch1 = 0;
uint16_t SpeedFromEEPROMch2 = 0;
uint16_t SpeedFromEEPROMch3 = 0;
uint16_t SpeedFromEEPROMch4 = 0;
uint16_t SpeedFromEEPROMch5 = 0;
uint16_t SpeedFromEEPROMch6 = 0;
uint16_t SpeedFromEEPROMch7 = 0;
*/

//Global variables
//Volatile variables (=variables that can suddenly change from for example interrupt) N.B. will not be optimized by the compiler
volatile uint8_t EchoON = ~0; //Echo on, for toggling the echo
volatile uint8_t RxBuffer[RxBufferLength];	//Buffer for intermediate storage of Rx-data | size: RxBufferLength
volatile uint8_t RecvCommand = 0;			//Flag indicating if a new command is received
volatile uint8_t NoBackspace;				//If flag raised, no backspace is allowed

//Ringbuffer
volatile uint8_t RingbufferHead = 0; //Buffer In
volatile uint8_t RingbufferTail = 0; //Buffer Out
uint8_t RingbufferFull = 0;			 //Buffer full flag


char TxBuffer[TxBufferLength]; //Buffer for intermediate storage of Tx-data | size: TxBufferLength
char ProcessedBuffer[MaxWordLength + 1]; //Adds place for "stop-bit" (Null-charachter)

const char commandArray[NoOfCommands][MaxCommandLength] = {"all","echo","read","fan","help","save","reset_eeprom","trend_data"};
enum commands{commandAll=1,commandEcho,commandRead,commandFanControll,commandHelp,commandSave,commandResetEEPROM,commandTrendData};
int mainmenu  = 1;
void UART_MENU(void){
	// If echo is on, and the ASCII character is higher than 31, or Bell, Carriage Return, Line Feed or backspace, then echo the character. Other are filtered as to not get strange behavior from Putty.
	if (RecvCommand) {

		UART_NewLine();

		if (ProcessWord() != 0) {
			switch (SearchForCommand()){
				case commandAll:
				if (ProcessWord() == 0){
					strcpy(TxBuffer,"All command with no arguments");
					UART_SendBuffer();
					}else{
					if (strcmp(ProcessedBuffer,"on")==0){
						AllON();
						break;
					}
					if (strcmp(ProcessedBuffer,"off")==0){
						AllOFF();
						break;
					}
					TxBuffer_StringWrite(TxBuffer,"All command with incorrect syntax");
					UART_NewLine();
				}
				break;
				
				case commandEcho:
				if (ProcessWord() == 0){
					
					UART_EchoCheck();
					UART_SendBuffer();
					}else{
					EchoToggle();
				}
				break;

				case commandRead:
				if (ProcessWord() == 0){
					TxBuffer_StringWrite(TxBuffer, "Read command with no arguments");
					}else{
					if(strcmp(ProcessedBuffer, "0") == 0){
						UART_ReadChannel(0);
						break;
					}
					else if(strcmp(ProcessedBuffer, "1") == 0){
						UART_ReadChannel(1);
						break;
					}
					else if(strcmp(ProcessedBuffer, "2") == 0){
						UART_ReadChannel(2);
						break;
					}
					else if(strcmp(ProcessedBuffer, "3") == 0){
						UART_ReadChannel(3);
						break;
					}
					else if(strcmp(ProcessedBuffer, "4") == 0){
						UART_ReadChannel(4);
						break;
					}
					else if(strcmp(ProcessedBuffer, "5") == 0){
						UART_ReadChannel(5);
						break;
					}
					else if(strcmp(ProcessedBuffer, "6") == 0){
						UART_ReadChannel(6);
						break;
					}
					else if(strcmp(ProcessedBuffer, "7") == 0){
						UART_ReadChannel(7);
						break;
					}
					else if(strcmp(ProcessedBuffer, "all") == 0){
						UART_ReadAll();
						break;
					}
					else{
						TxBuffer_StringWrite(TxBuffer,"Read with incorrect syntax");
						UART_NewLine();
					}
				}
				break;
				
				case commandFanControll:
				if (ProcessWord() == 0){
					TxBuffer_StringWrite(TxBuffer,"Fan command with no arguments");
					}else{
					
					if(strcmp(ProcessedBuffer, "0") == 0){
						speedch0 = UART_FanSpeedSet();
						AnalogWrite(0, speedch0);
						TxBuffer_IntWrite(TxBuffer, speedch0);
						break;
					}
					else if(strcmp(ProcessedBuffer, "1") == 0){
						speedch1 = UART_FanSpeedSet();
						AnalogWrite(1, speedch1);
						TxBuffer_IntWrite(TxBuffer, speedch1);
						break;
					}
					else if(strcmp(ProcessedBuffer, "2") == 0){
						speedch2 = UART_FanSpeedSet();
						AnalogWrite(2, speedch2);
						TxBuffer_IntWrite(TxBuffer, speedch2);
						break;
					}
					else if(strcmp(ProcessedBuffer, "3") == 0){
						speedch3 = UART_FanSpeedSet();
						AnalogWrite(3, speedch3);
						TxBuffer_IntWrite(TxBuffer, speedch3);
						break;
					}
					else if(strcmp(ProcessedBuffer, "4") == 0){
						speedch4 = UART_FanSpeedSet();
						AnalogWrite(4, speedch4);
						TxBuffer_IntWrite(TxBuffer, speedch4);
						break;
					}
					else if(strcmp(ProcessedBuffer, "5") == 0){
						speedch5 = UART_FanSpeedSet();
						AnalogWrite(5, speedch5);
						TxBuffer_IntWrite(TxBuffer, speedch5);
						break;
					}
					else if(strcmp(ProcessedBuffer, "6") == 0){
						speedch6 = UART_FanSpeedSet();
						AnalogWrite(6, speedch6);
						TxBuffer_IntWrite(TxBuffer, speedch6);
						break;
					}
					else if(strcmp(ProcessedBuffer, "7") == 0){
						speedch7 = UART_FanSpeedSet();
						AnalogWrite(7, speedch7);
						TxBuffer_IntWrite(TxBuffer, speedch7);
						break;
					}
					else if(strcmp(ProcessedBuffer, "all") == 0){
					
						break;
						}else{
						
						strcpy(TxBuffer,"Fan with incorrect syntax");
						UART_SendBuffer();
						UART_NewLine();
					}
				}
				break;
				
				case commandHelp:
				
				break;
				
				case commandSave:
				//Turns the fans up to max speed
				AllON();
				
				UART_NewLine();
				
				//Read RPM values from trend data in EEPROM
				SpeedFromEEPROMch[0] = my_eeprom_read_char(fan_0_EEPROM_adr);
				SpeedFromEEPROMch[1] = my_eeprom_read_char(fan_1_EEPROM_adr);
				SpeedFromEEPROMch[2] = my_eeprom_read_char(fan_2_EEPROM_adr);
				SpeedFromEEPROMch[3] = my_eeprom_read_char(fan_3_EEPROM_adr);
				SpeedFromEEPROMch[4] = my_eeprom_read_char(fan_4_EEPROM_adr);
				SpeedFromEEPROMch[5] = my_eeprom_read_char(fan_5_EEPROM_adr);
				SpeedFromEEPROMch[6] = my_eeprom_read_char(fan_6_EEPROM_adr);
				SpeedFromEEPROMch[7] = my_eeprom_read_char(fan_7_EEPROM_adr);
				
				//
				for (int i = 0; i<=7; i++)
				{
					SpeedToEEPROMch[i] = Tacho_filter(i);	//Reading the rpm value for the tachometer
					
					if (SpeedFromEEPROMch[i] != 0)
					{
						SpeedToEEPROMch[i] = ((19*SpeedFromEEPROMch[i]/20) + (SpeedToEEPROMch[i]/20));	//Calculating new value for EEPROM
						
						if ((SpeedFromEEPROMch[i]*0.99) > SpeedToEEPROMch[i])	//Predicting fault in fan
						{
							TxBuffer_StringWrite(TxBuffer,"Fault predicted in fan ");
							TxBuffer_IntWrite(TxBuffer, i);
							UART_NewLine();
						}
					}
				}
				
				//Updates values to EEPROM
				my_eeprom_update_char(fan_0_EEPROM_adr, SpeedToEEPROMch[0]);
				my_eeprom_update_char(fan_1_EEPROM_adr, SpeedToEEPROMch[1]);
				my_eeprom_update_char(fan_2_EEPROM_adr, SpeedToEEPROMch[2]);
				my_eeprom_update_char(fan_3_EEPROM_adr, SpeedToEEPROMch[3]);
				my_eeprom_update_char(fan_4_EEPROM_adr, SpeedToEEPROMch[4]);
				my_eeprom_update_char(fan_5_EEPROM_adr, SpeedToEEPROMch[5]);
				my_eeprom_update_char(fan_6_EEPROM_adr, SpeedToEEPROMch[6]);
				my_eeprom_update_char(fan_7_EEPROM_adr, SpeedToEEPROMch[7]);
				TxBuffer_StringWrite(TxBuffer,"Saving done");
				break;
				
				case commandResetEEPROM:
				//Sets value to zero at every EEPROM adresse
				my_eeprom_update_char(fan_0_EEPROM_adr, 0);
				my_eeprom_update_char(fan_1_EEPROM_adr, 0);
				my_eeprom_update_char(fan_2_EEPROM_adr, 0);
				my_eeprom_update_char(fan_3_EEPROM_adr, 0);
				my_eeprom_update_char(fan_4_EEPROM_adr, 0);
				my_eeprom_update_char(fan_5_EEPROM_adr, 0);
				my_eeprom_update_char(fan_6_EEPROM_adr, 0);
				my_eeprom_update_char(fan_7_EEPROM_adr, 0);
				
				TxBuffer_StringWrite(TxBuffer, "EEPROM has been reset");
				break;
				
				case commandTrendData:
				//Read RPM values from trend data in EEPROM
				SpeedFromEEPROMch[0] = my_eeprom_read_char(fan_0_EEPROM_adr);
				SpeedFromEEPROMch[1] = my_eeprom_read_char(fan_1_EEPROM_adr);
				SpeedFromEEPROMch[2] = my_eeprom_read_char(fan_2_EEPROM_adr);
				SpeedFromEEPROMch[3] = my_eeprom_read_char(fan_3_EEPROM_adr);
				SpeedFromEEPROMch[4] = my_eeprom_read_char(fan_4_EEPROM_adr);
				SpeedFromEEPROMch[5] = my_eeprom_read_char(fan_5_EEPROM_adr);
				SpeedFromEEPROMch[6] = my_eeprom_read_char(fan_6_EEPROM_adr);
				SpeedFromEEPROMch[7] = my_eeprom_read_char(fan_7_EEPROM_adr);
				
				//Presenting the data in the UART menu
				UART_NewLine();
				for (int i = 0; i<=7; i++)
				{
					TxBuffer_StringWrite(TxBuffer, "Trend data for fan ");
					TxBuffer_IntWrite(TxBuffer, i);
					TxBuffer_StringWrite(TxBuffer, ": ");
					TxBuffer_IntWrite(TxBuffer, SpeedFromEEPROMch[i]);
					UART_NewLine();
				}
				break;
				
				default:
				TxBuffer_StringWrite(TxBuffer, "Unknown command!");
			}
		}
		ResetSHell();
	}
	
}
void UART_HelpMenu(void){
	strcpy(TxBuffer,"Available commands:");
	UART_SendBuffer();
	UART_NewLine();
	strcpy(TxBuffer,"All, command for toggle ");
	UART_SendBuffer();
	UART_NewLine();
}
void UART_Echo(char recvbyte){	
	
	if(((recvbyte > 31) && (recvbyte != backspace127)) || (recvbyte == Bell) || (recvbyte == CR) || (recvbyte == LF)){
		if(RingbufferFull){
			UART_TX_BUFFER = Bell;								//Writes bell(end of line alert)
			} else {
			UART_TX_BUFFER = recvbyte;							//Puts the received byte in the tx-buffer
		}
		NoBackspace = 0;										//Lower the NoBackspace FLag
	}
	if((recvbyte == backspace) || (recvbyte == backspace127)) { //Check if backspace is pressed
		if(RingbufferTail == RingbufferHead){				    //Check if buffer is empty
			if(NoBackspace == 0){								//Check if NoBackspace flag is low, if so raise it
				NoBackspace = 1;                                //The flag is to disable the user from using backspace
				UART_TX_BUFFER = recvbyte;						//Sends the received byte since | backspace
				} else {
				UART_TX_BUFFER = Bell;
			}
			} else {
			UART_TX_BUFFER = recvbyte;							//Sends the recieved byte, thats not the ones checked above
		}
	}
}

//Function for finding the incoming command
char SearchForCommand(void){
	for(uint8_t CompareCommand = 0; CompareCommand < NoOfCommands; CompareCommand++){ //Searching command for command in the commandarray
		for(uint8_t CompareChar = 0; CompareChar < MaxCommandLength; CompareChar++){  //Search character for character in the commandarray
			if(commandArray[CompareCommand][CompareChar] == ProcessedBuffer[CompareChar]){ //Check if the command in the received data is equal to commands that are available
				if(ProcessedBuffer[CompareChar] == 0){
					return CompareCommand + 1;
				}
				} else {
				CompareChar = MaxCommandLength;
			}
		}
	}
	return 0; //Process done
}

void AllON(void){
	for(int i = 0; i < 8; i++){
		AnalogWrite(i,0);
	}
	TxBuffer_StringWrite(TxBuffer,"PWM off all fans, fans full power");
}
void AllOFF(void){
	for(int j = 0; j < 8; j++){
		AnalogWrite(j,100);
	}
	TxBuffer_StringWrite(TxBuffer,"PWM fully on all fans, fans off");
}

//Function for processing the characters, so its easy to check for correct commands
char ProcessWord(void){
	for(uint8_t LoopNo = 0; LoopNo < MaxWordLength; LoopNo++){ //Scroll through, to ensure that all characters in buffer is checked
		uint8_t RxChar; //Make a temporary Rx character to check each character in buffer
		RxChar = UART_getRxBufferChar(); //Gets characters from the rx buffer via UART_getRXBufferChar();
		if(RxChar == 0 && LoopNo == 0){ //Check if no char is received
			return 0; //Ready for checking
		}
		if(RxChar > 32){
			ProcessedBuffer[LoopNo] = RxChar; //Puts the characters in a buffer for the Processed Characters
			} else {
			ProcessedBuffer[LoopNo] = 0; //Top ups the buffer with a "Stop-bit" (Null-character)
			break; //exit loop
		}
	}
	ProcessedBuffer[MaxWordLength] = 0; //Make sure the end has an null character
	return 1; //Process done
}

//Function for adding character in to the rx-buffer
void CharInBUF(char incomingchar){
	
	if(incomingchar == backspace || incomingchar == backspace127){
		if(RingbufferHead != RingbufferTail){ //Check if its not empty
			if(RingbufferHead == 0){
				RingbufferHead = RxBufferLength - 1;
				} else {
				RingbufferHead--; //Decrease buffer index, since it is being emptied | move one character back until emtied
			}
			RingbufferFull = FALSE;	//Lower the flag, since it is empty
		}
		} else {
		
		if(RingbufferFull){ //Check if the the flag raise is true (if its still full)
			if((RingbufferTail - RingbufferHead == 1) || ((RingbufferHead == RxBufferLength - 1) && (RingbufferTail == 0))){
				//Buffer is full, the byte gets ignored
				} else {
				//The claim is false, change flag back | not full
				RingbufferFull = FALSE;
			}
		}
		//If the rxbuffer is not full, put a char into it
		if(!RingbufferFull){
			RxBuffer[RingbufferHead] = incomingchar; //Put char in to buffer
			//Checks again if the buffer is full, since a new character is put in it
			if((RingbufferTail - RingbufferHead == 1) || ((RingbufferHead == RxBufferLength - 1) && (RingbufferTail == 0))){
				RingbufferFull = TRUE; //Buffer is full, and the full flag will be raised
				} else {
				RingbufferHead++; //If its not full, increase the index of the buffer, since a new character is put in to it
				
				//If the buffer has reached the end, wrap around
				if(RingbufferHead >= RxBufferLength){
					RingbufferHead = 0; //Buffer wrapped around
				}
			}
		}
	}
}

//Function to extract character for character from the Rx-buffer
char UART_getRxBufferChar(void){
	char BufferChar; //Temporary character to return
	if(RingbufferHead == RingbufferTail){ //Check if the buffer is empty, return 0 if true
		return 0;
		} else {
		BufferChar = RxBuffer[RingbufferTail]; //Saves each char in buffer to return
		RingbufferTail++;					   //Increment tail, since characters is extracted from buffer
		if (RingbufferTail >= RxBufferLength){ //Is all characters sent, if so reset buffer
			RingbufferTail = 0;				   //Reset buffer
		}
		return BufferChar; //Return buffer, one character at the time
	}
}

//Reset the buffer values
void Ringbuffer_Reset(void){
	RingbufferHead = 0;
	RingbufferTail = 0;
	RingbufferFull = 0;
}

//Function for new line, by sending both CR and LF | Next line, cursor position leftmost
void UART_NewLine(void){
	UART_SendChar(CR);
	UART_SendChar(LF);
}

//Send one character at a time
void UART_SendChar(char CharToSend){
	while(!TxBufferFlag){ //Checking the Tx-Flag
		//Loop until buffer is empty
	}
	UART_TX_BUFFER = CharToSend; //When Tx-buffer is ready, send data to buffer
}

//Function for sending the whole buffer (strings)
void UART_SendBuffer(void){
	for(uint8_t SendLoop = 0; SendLoop < TxBufferLength; SendLoop++){ //Loops trough the whole tx-buffer, to ensure that all characters will be sent
		char TxChar;				 //Saves each character in the buffer, in a temporary tx-buffer character
		TxChar = TxBuffer[SendLoop]; //Saves one character at the time
		if (TxChar!=0){				 //Checks for the "stop-bit" in the temporary character
			UART_SendChar(TxChar);	 //Sends the data as long it has not reached the end
			}else{
			return;					 //Return out of loop, when the "stop-bit" comes
		}
	}
}

//Change internal oscillator to 16MHz
void Osc_init(void){
	CCP = 0xD8; //Unlock protected registers, to allow change of internal oscillator
	CLKCTRL_OSCHFCTRLA = (3 << CLKCTRL_FREQSEL0_bp ); //Set internal clock oscillator to 16MHz
}

int UART_FanSpeedSet(){
	int channel = 0;
	channel = ProcessedBuffer[0] - '0';
	if(ProcessWord() != 0){
		int numofdigits = 0;
		
		numofdigits = UART_DigitsCheck();

		
		if (numofdigits == 1){
			if(ProcessedBuffer[0] == 0){
				strcpy(TxBuffer,"Argument is 0, PWM off, fan full power");
				UART_SendBuffer();
				UART_NewLine();
			}
			speed = UART_CheckFanInput(1);
		}
		if(numofdigits == 2){
			if(ProcessedBuffer[0] == 0){
				strcpy(TxBuffer,"Argument can't start with 0");
				UART_SendBuffer();
				UART_NewLine();
				} else {
				
				speed = UART_CheckFanInput(2);
				
			}
		}
		if(numofdigits == 3){
			speed = UART_CheckFanInput(3);
			
		}
		sprintf(TxBuffer,"Fan %u",channel);
		UART_SendBuffer();
		strcpy(TxBuffer," written to: ");
		UART_SendBuffer();

		} else {
		strcpy(TxBuffer,"No number argument");
		UART_SendBuffer();
	}
	return speed;
}
int UART_DigitsCheck(void){
	int numofdigits = 0;
	
	for(int i = 0; ProcessedBuffer[i] != 0; i++){
		ProcessedBuffer[i] = ProcessedBuffer[i] - '0';
		numofdigits++;
	}
	return numofdigits;
}

int UART_CheckFanInput(char digits){
	int tempspeed = 0;
	
	if(digits == 1){
		n1 = NumCheck(0);
		
		tempspeed = n1;
		} else if (digits == 2) {
		n1 = NumCheck(0);
		n2 = NumCheck(1);
		tempspeed = n1*10 + n2;
		} else if (digits == 3){
		n1 = NumCheck(0);
		n2 = NumCheck(1);
		if(n1 != 1 || n2 != 0){
			strcpy(TxBuffer,"Number is greater then 100 or not a number");
			UART_SendBuffer();
			UART_NewLine();
			strcpy(TxBuffer,"Put in a number between 0 and 100");
			UART_SendBuffer();
			UART_NewLine();
			tempspeed = 404;
			} else {
			tempspeed = 100;
			strcpy(TxBuffer,"Argument is 100, PWM is fully on");
			UART_SendBuffer();
			UART_NewLine();
		}
	}
	if (tempspeed > 100){
		strcpy(TxBuffer,"Wrong Input");
		UART_SendBuffer();
		UART_NewLine();
		strcpy(TxBuffer,"Try again");
		UART_SendBuffer();
		UART_NewLine();
		} else {
		speed = tempspeed;
		
	}
	return speed;
}
void UART_EchoCheck(void){
	if (EchoON){
		strcpy(TxBuffer,"Echo is on");
		}else{
		strcpy(TxBuffer,"Echo is off");
	}
}
void ResetSHell(){
	UART_NewLine();
	strcpy(TxBuffer,"Write an command: ");
	UART_SendBuffer();
	Ringbuffer_Reset();
	NoBackspace = 1;
	RecvCommand = 0;
}
int NumCheck(int num){
	int n = 0;
	
	if (!(ProcessedBuffer[num] <= 9 && ProcessedBuffer[num] >= 0)){
		strcpy(TxBuffer,"Please enter a number");
		UART_SendBuffer();
		UART_NewLine();
		n = 404;
		} else {
		n = ProcessedBuffer[num];
	}
	return n;
}
void UART_ReadChannel(char ch){
	uint32_t tacho = 0;
	int count = 0;
	int left_shift = 0;
	int right_shift = 0;
	
	tacho = Tacho_filter(ch);

	//Function for finding the length of an integer
	while(tacho!=0) //Run until tacho value is zero
	{
		tacho=tacho/10; //divide until tacho is 0,....
		count++;		//Count for each round to get the length
	}
	
	//Title
	if(count == 0){
		left_shift = count;
		right_shift = left_shift + 1;
	}
	else {
		left_shift = (count-3);
		right_shift = left_shift + 2;
	}
	UART_NewLine();
	for(int k = 0; k <= (left_shift);k++ ){
		
		strcpy(TxBuffer," ");
		UART_SendBuffer();
		
	}
	strcpy(TxBuffer," TACHO - VALUES");
	UART_SendBuffer();
	for(int k = 0; k <= right_shift;k++ ){
		
		strcpy(TxBuffer," ");
		UART_SendBuffer();
		
	}
	
	strcpy(TxBuffer,"|");
	UART_SendBuffer();
	UART_NewLine();
	for(int k = 0; k <= left_shift;k++ ){
		
		strcpy(TxBuffer," ");
		UART_SendBuffer();
		
	}
	strcpy(TxBuffer,"  FAN CHANNEL ");
	UART_SendBuffer();
	sprintf(TxBuffer,"%u",ch);
	UART_SendBuffer();
	
	for(int k = 0; k <= right_shift;k++ ){
		
		strcpy(TxBuffer," ");
		UART_SendBuffer();
		
	}
	
	strcpy(TxBuffer,"|");
	UART_SendBuffer();
	UART_NewLine();
	for(int loop = 0; loop < 11; loop++){
		
		strcpy(TxBuffer,"Channel ");
		UART_SendBuffer();
		sprintf(TxBuffer,"%u",ch);
		UART_SendBuffer();
		strcpy(TxBuffer," value: ");
		UART_SendBuffer();
		tacho = Read_Tacho(ch);
		sprintf(TxBuffer,"%lu",tacho);
		UART_SendBuffer();
		strcpy(TxBuffer,"|");
		UART_SendBuffer();
		UART_NewLine();
		
	}
	
	
}

void UART_ReadAll(void){
	//Title
	UART_TitleAll();
	//FIRST ROW
	UART_Row(0,5);
	//SECOND ROW
	UART_Row(1,5);
}

void UART_TitleAll(void){
	UART_NewLine();
	strcpy(TxBuffer,"  TACHO - VALUES   |");
	UART_SendBuffer();
	strcpy(TxBuffer,"      ALL FANS      |");
	UART_SendBuffer();
	UART_NewLine();
	UART_NewLine();
}

void UART_Row(char row, char numofsamples){
	uint32_t tacho = 0;
	for(int ch = 0; ch <= 3; ch++){
		strcpy(TxBuffer,"-------------------- ");
		UART_SendBuffer();
	}
	UART_NewLine();
	row = row*5-(row-1);
	for(int ch = 0+row; ch <= 3+row; ch++){
		
		strcpy(TxBuffer,"Channel ");
		UART_SendBuffer();
		sprintf(TxBuffer,"%u",ch);
		UART_SendBuffer();
		strcpy(TxBuffer,"          | ");
		UART_SendBuffer();
	}
	UART_NewLine();
	for(int samples = 0; samples < numofsamples; samples++){
		for(int ch = 0+row; ch <= 3+row;ch++){
			strcpy(TxBuffer,"Channel ");
			UART_SendBuffer();
			sprintf(TxBuffer,"%u",ch);
			UART_SendBuffer();
			tacho = Tacho_filter(ch);
			strcpy(TxBuffer," value: ");
			UART_SendBuffer();
			sprintf(TxBuffer,"%lu",tacho);
			UART_SendBuffer();
			strcpy(TxBuffer," | ");
			UART_SendBuffer();
		}
		UART_NewLine();
	}
	UART_NewLine();
}
int Tacho_filter(char ch){ //Need too "filter" out the first value since the first reading is always wrong
	uint32_t filter_val = 0;
	filter_val = Read_Tacho(ch);
	_delay_ms(40);
	filter_val = Read_Tacho(ch);
	return filter_val;
}


//UART3 setup
void UART3_init(void){
	/*Calculating the baudrate from the formula in datasheet.
	The register is 16 bit therfore uint16_t*/
	const uint16_t BAUD_rate = (uint16_t) (F_CPU*64/(16*UART_bps));
	
	//Set the baudrate to the UART3.BAUD register
	USART3.BAUD = BAUD_rate;
	
	//Setting up PB0(TxD)
	//Set PB0 to TxD, PB1 to RxD, PB2 to XCK and PB3 as XDIR
	PORTMUX.USARTROUTEA = PORTMUX_USART3_DEFAULT_gc;
	//Set PB0 to output
	PORTB.DIR |= (1 << TxD);

	//Enable RxD interrupt
	USART3.CTRLA = (1 << USART_RXCIE_bp);
	
	//Enable RxD and TxD
	USART3.CTRLB = (1 << USART_RXEN_bp) | (1 << USART_TXEN_bp);
	
	//Setting frame format and modes | Asynchronus (UART) | Disable Parity bit | Set 1 stop bit | Rx/Tx-Data size 8-bit
	USART3.CTRLC = USART_CMODE_ASYNCHRONOUS_gc | USART_PMODE_DISABLED_gc | USART_SBMODE_1BIT_gc | USART_CHSIZE_8BIT_gc;
	
	USART3.CTRLD = USART_ABW_WDW0_gc;
}

void ISR_Func(void){
	uint8_t recvbyte = UART_RX_BUFFER;
	static uint8_t lastbyte = 0;			//Static does that the variable not initialize more then one time
	switch(recvbyte){						//Switch case for checking the incoming for bytes
		case(0):
		EchoON = ~EchoON;					//If case is 0, toggle EchoON
		break;								//Exit loop
		case(LF):
		if (lastbyte == CR){				//ignore LF if last byte was CR
			break;							//Exit loop, since we ignore it
		}
		case(CR):							//When CR(or keyboard <enter>) is reachieved, the command can be run
		RecvCommand = 1;					//The received command flag is raised, so the process with perform task is started
		default:							//If no cases are true, the defaults run | default case dont need any break to exit
		CharInBUF(recvbyte);				//Character is put in the buffer, through CharINBUF();
	}
	if(EchoON && TxBufferFlag){				//Checks if echo is on and tx-buffer is ready to receive data
		UART_Echo(recvbyte);				//Sends echo to the terminal via tx-buffer
	}
	lastbyte = recvbyte;					//Saves the received byte to lastbyte for next round
}



void TxBuffer_StringWrite(char* buf, char* str){
	strcpy(buf,str);
	UART_SendBuffer();
}

void TxBuffer_IntWrite(char* buf, int data){
	sprintf(buf,"%u",data);
	UART_SendBuffer();
}

void Buffer_init(void){
	RxBuffer[0] = 0; //Setting stopbit of Rxbuffer
}

void EchoToggle(void){
	if (strcmp(ProcessedBuffer,"on")==0){
		EchoON = ~0;
		strcpy(TxBuffer,"Echo on");
		UART_SendBuffer();
		
	}
	else if (strcmp(ProcessedBuffer,"off")==0){
		EchoON = 0;
		strcpy(TxBuffer,"Echo off");
		UART_SendBuffer();
		
		} else {
		strcpy(TxBuffer,"echo with incorrect syntax ");
		UART_SendBuffer();
		strcpy(TxBuffer,ProcessedBuffer);
		UART_SendBuffer();
		UART_NewLine();
	}
}
