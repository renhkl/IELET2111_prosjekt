/*
 * i2c2.c
 *
 * Created: 08.05.2022 15:02:45
 * Author : renat
 *
 *Inspirasjon/kilder:
 *Databladet
 *https://github.com/MicrochipTech/avr128da48-i2c-send-receive-example/blob/master/avr128da48-i2c-send-receive-example/I2C_example/main.c
 */ 


#define F_CPU 4000000
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

#define CLK_PER 4000000
#define I2C_SCL_FREQ 100000 //standard mode

#define SDA_PIN 2
#define SCL_PIN 3


//
#define TWI1_BAUD(F_SCL, T_RISE) ( ((float)CLK_PER/(2*(float)F_SCL)) - (5 + ( ((float)CLK_PER * (float)T_RISE) /2) ) )

//slaveadresser
#define TEMP_ADDRESS 0x38
#define LCD_ADDRESS 0x27


//settes i MADDR
#define I2C_DIRECTION_BIT_WRITE 0
#define I2C_DIRECTION_BIT_READ 1
#define I2C_SET_ADDR_POSITON(slaveAddress) (slaveAddress << 1)

//dette gjelder for den ene retningen men hvilken?
#define I2C_SLAVE_RESPONSE_ACKED (!(TWI_RXACK_bm & TWI1_MSTATUS))
#define I2C_DATA_RECEIVED (TWI_RIF_bm & TWI1_MSTATUS) //kan bruke flaggene uten interrupts. det finnes ikke RIF for i SSTATUS. hva betyr??

#define I2C_MASTER_RESPONSE_ACKED (!(TWI_RXACK_bm & TWI1_SSTATUS))

#define TEMP_CONVERTER(SIGNAL) ((SIGNAL/1048576) * 200 - 50) 

static void i2c_init(void);
static void i2c_transmitAddrPacket(uint8_t slaveAddress, uint8_t directionBit);

static uint8_t i2c_receiveDataPacket(void);
//static uint8_t i2c_
static void i2c_transmitDataPacket(uint8_t data);
//en for å sende datapakker også
static void i2c_sendMasterCommand(uint8_t masterCommand);
//en for at slaven også kan sende kommandoer, litt andre da
static void i2c_masterACKaction(void);
static void i2c_masterNACKaction(void);
//de to finnes for slave også

//static void i2c_slaveACKaction(void);
//static void i2c_slaveNACKaction(void);


	 //så dataene som skal leses i i2c er volatile?
	 volatile uint8_t statusData = 0;
	 volatile uint8_t humidityUpperDatabyte = 0;
	 volatile uint8_t humidityMiddleDatabyte = 0;
	 volatile uint8_t humidityLowerandTempUpperDatabyte = 0;
	 volatile uint8_t tempMiddleDatabyte = 0;
	 volatile uint8_t tempLowerDatabyte = 0;
	 //volatile uint32_t humidity20BitValue = 0;
	 volatile uint32_t temp20BitValue = 0;
	 float temperature = 0;  //vet ikke hvor stort det må være for regnestykke



int main(void){
	//settte til inputs?
	PORTF.DIR &= ~(1 << SDA_PIN);
	PORTF.DIR &= ~(1 << SCL_PIN);
	PORTF.PIN2CTRL |= (1 << PORT_PULLUPEN_bp);
	PORTF.PIN3CTRL |= (1 << PORT_PULLUPEN_bp);
	//kan bruke twi0 på pa2 og pa3??
	

	
    i2c_init(); //så baud rate og det der sendes en gang*
    while (1)  { 
		//skrive først i stedet for å kunne sende kommandoer
		_delay_ms(20); //ATH10 trenger 20 ms før kontakt fra masteren etter idle-bussen
		i2c_transmitAddrPacket(TEMP_ADDRESS, I2C_DIRECTION_BIT_WRITE);
		//i2c_transmitDataPacket(0xe1); //initialisere temperatursensoren 
		i2c_transmitDataPacket(0xac); //ber tempsensoren om å beregne fuktighet og temperatur
		_delay_ms(75);
		i2c_transmitDataPacket(0x33); //data0
		i2c_transmitDataPacket(0x0); //data1
		i2c_sendMasterCommand(~TWI_MCMD_RECVTRANS_gc);  //mellom hver?
		i2c_masterNACKaction(); //blir nack før den avsluttes
		i2c_sendMasterCommand(TWI_MCMD_STOP_gc);
		
		
		
		//endrer retningen 
		i2c_transmitAddrPacket(TEMP_ADDRESS, I2C_DIRECTION_BIT_READ); //tilsvarer initialization biten
		statusData = i2c_receiveDataPacket(); //stopper her !!
		i2c_masterACKaction();
		
		i2c_sendMasterCommand(TWI_MCMD_RECVTRANS_gc);  //mellom hver?
		humidityUpperDatabyte = i2c_receiveDataPacket();
		i2c_masterACKaction();
		i2c_sendMasterCommand(TWI_MCMD_RECVTRANS_gc);  //mellom hver?
		humidityMiddleDatabyte = i2c_receiveDataPacket();
		i2c_masterACKaction();
		i2c_sendMasterCommand(TWI_MCMD_RECVTRANS_gc);  //mellom hver?
		humidityLowerandTempUpperDatabyte = i2c_receiveDataPacket();
		i2c_masterACKaction();
		i2c_sendMasterCommand(TWI_MCMD_RECVTRANS_gc);  //mellom hver?
		tempMiddleDatabyte = i2c_receiveDataPacket();
		i2c_masterACKaction();
		i2c_sendMasterCommand(TWI_MCMD_RECVTRANS_gc);  //mellom hver?
		tempLowerDatabyte = i2c_receiveDataPacket();
		
		
		//i2c_sendMasterCommand(TWI_MCMD_RECVTRANS_gc);  //mellom hver?
		//tempValue = i2c_receiveDataPacket();
		
		i2c_masterNACKaction(); //blir nack før den avsluttes
		i2c_sendMasterCommand(TWI_MCMD_STOP_gc);
		
		//sette sammen og dele opp verdiene

		//uint8_t humidityLowerDatabyte = humidityLowerandTempUpperDatabyte & 00001111;
		uint8_t tempUpperDatabyte = humidityLowerandTempUpperDatabyte & 11110000;
		tempUpperDatabyte = (tempUpperDatabyte >> 4);
		temp20BitValue = (tempUpperDatabyte << 8) | tempMiddleDatabyte;
		temp20BitValue = (temp20BitValue << 8) | tempLowerDatabyte;
		temperature = TEMP_CONVERTER(temp20BitValue);
		_delay_ms(500);
		
    }
}

//eget datasheet i tillegg til greia, ikke bare kommuniksjonen


static void i2c_init(void){
	TWI1_MBAUD = (uint8_t)TWI1_BAUD(I2C_SCL_FREQ, 0.00025);
	TWI1_MCTRLA = TWI_ENABLE_bm;
	TWI1_MSTATUS = TWI_BUSSTATE_IDLE_gc;
}

static void i2c_transmitAddrPacket(uint8_t slaveAddress, uint8_t directionBit){
	TWI1_MADDR = I2C_SET_ADDR_POSITON(slaveAddress) + directionBit;
	while(!I2C_SLAVE_RESPONSE_ACKED){
		;
	}
}

//for den motsatte av denne må det også være en ACK-sjekk


static void i2c_transmitDataPacket(uint8_t data){
	TWI1_MDATA |= data;
	while(!I2C_SLAVE_RESPONSE_ACKED){
		;
	}
	
}

//denne er den eneste med value
static uint8_t i2c_receiveDataPacket(void){
	while(!I2C_DATA_RECEIVED){
		;
		}
		return TWI1.MDATA; //hvorfor Mdata?
}

//må være en sjekk for motsatt av denne også

static void i2c_sendMasterCommand(uint8_t masterCommand){
	TWI1_MCTRLB |= masterCommand;
}

static void i2c_masterACKaction(void){
	TWI1_MCTRLB &= ~TWI_ACKACT_bm;
}

static void i2c_masterNACKaction(void){
	TWI1_MCTRLB	|= TWI_ACKACT_bm;
}

//static void i2c_slaveACKaction(void){
	
//}

//static void i2c_slaveNACKaction(void){
	
//}