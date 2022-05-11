/*
 * CFile1.c
 *
 * Created: 10.04.2022 12:11:51
 *  Author: Idar Evenstad
 */ 

#include "AVR_Analog_RW.h"

void ADC_Init(){
	VREF.ADC0REF = VREF_REFSEL_VDD_gc; //Set the Voltage reference to VDD(here: 5v)
	ADC0.CTRLA = ADC_RESSEL_10BIT_gc |	ADC_ENABLE_bm; //Set to 10-bit mode(1024) and enables ADC
	ADC0.CTRLC = ADC_PRESC_DIV4_gc; //Set ADC prescaler, can be used to divide clock freq
}

uint16_t AnalogRead(char port){
	ADC0.MUXPOS = port; //choose between port PD0-PF5(AIN0-AIN21) or defined analog ports
	ADC0.COMMAND = ADC_STCONV_bm; //Start analog converting
	uint16_t analog_value; //Variable for saving analog value
	while (!(ADC0.INTFLAGS & ADC_RESRDY_bm)); //While loop for checking the interrupt flag if the converting is done
	analog_value = (uint16_t)ADC0.RES; //Read the analog value + clear interrupt flag
	return analog_value; //Return the analog value
}

//Function for analogwrite to pin with duty 0 - 100%
void AnalogWrite(char ch, char duty){
	if (ch == 0){
		TCA0.SPLIT.LCMP2 = dutycalc(duty);
		PORTA.DIR |= PIN2_bm;
	}
	if (ch == 1){
		TCA0.SPLIT.HCMP0 = dutycalc(duty);
		PORTA.DIR |= PIN3_bm;
	}
	if (ch == 2){
		TCA0.SPLIT.HCMP1 = dutycalc(duty);
		PORTA.DIR |= PIN4_bm;
	}
	if (ch == 3){
		TCA0.SPLIT.HCMP2 = dutycalc(duty);
		PORTA.DIR |= PIN5_bm;
	}
	if (ch == 4){
		TCA1.SPLIT.LCMP2 = dutycalc(duty);
		PORTB.DIR |= PIN2_bm;
	}
	if (ch == 5){
		TCA1.SPLIT.HCMP0 = dutycalc(duty);
		PORTB.DIR |= PIN3_bm;
	}
	if (ch == 6){
		
		TCA1.SPLIT.HCMP1 = dutycalc(duty);
		PORTB.DIR |= PIN4_bm;
	}
	if (ch == 7){
		TCA1.SPLIT.HCMP2 = dutycalc(duty);
		PORTB.DIR |= PIN5_bm;
	}


}
//calculating duty from 0 - 100%
uint8_t dutycalc(uint8_t pre){
	uint8_t duty = 0xFF*(pre*0.01);
	return duty;
}
//Read frequency 
uint32_t Read_Tacho(char ch){
	uint32_t tacho_val = 0;
	uint32_t rpm = 0;
	//Changing event check channel
	if(ch == 0){
		EVSYS.CHANNEL2 = EVSYS_CHANNEL2_PORTC_PIN0_gc; //Checks event channel 0
	}
	else if (ch == 1){
		EVSYS.CHANNEL2 = EVSYS_CHANNEL2_PORTC_PIN1_gc; //Checks event channel 1
	}
	else if (ch == 2){
		EVSYS.CHANNEL2 = EVSYS_CHANNEL2_PORTC_PIN2_gc; //Checks event channel 2
	}
	else if (ch == 3){
		EVSYS.CHANNEL2 = EVSYS_CHANNEL2_PORTC_PIN3_gc; //Checks event channel 3
	}
	else if (ch == 4){
		EVSYS.CHANNEL2 = EVSYS_CHANNEL2_PORTC_PIN4_gc; //Checks event channel 4
	}
	else if (ch == 5){
		EVSYS.CHANNEL2 = EVSYS_CHANNEL2_PORTC_PIN5_gc; //Checks event channel 5
	}
	else if (ch == 6){
		EVSYS.CHANNEL2 = EVSYS_CHANNEL2_PORTC_PIN6_gc; //Checks event channel 6
	}
	else if (ch == 7){
		EVSYS.CHANNEL2 = EVSYS_CHANNEL2_PORTC_PIN7_gc; //Checks event channel 7
	}
	
	
	EVSYS.USERTCB0CAPT = EVSYS_USER_CHANNEL2_gc;
	tacho_val = TCB0.CCMP; //Reads the frequency of tacho from the TCB capture/compare register
	
	//Remove extreme values when tacho value is 0 and denominator is 0, aka. improper fraction
	if(tacho_val == 0){
		rpm = 0;
		}else {
		rpm = (1600000*60)/(tacho_val*2); //Calculates RPM from freqency
	}
	TCB0.CCMP = 0x0000; //Resets the compare value
	return rpm;
}

void PWM_init(){
	TCA1_init();
	TCA0_init();
}

void TCA0_init(){
	PORTMUX.TCAROUTEA |= PORTMUX_TCA0_PORTA_gc;
	TCA0.SPLIT.CTRLD = TCA_SPLIT_SPLITM_bm;	//Enable splitmode
	TCA0.SPLIT.HPER = 0xFF; //Set period Lowbits
	TCA0.SPLIT.LPER = 0xFF; //Set period Highbits
	TCA0.SPLIT.CTRLA = TCA_SPLIT_ENABLE_bm | TCA_SPLIT_CLKSEL_DIV16_gc; //Enable clock and set prescaler (crystal clk/16)
	TCA0.SPLIT.CTRLB = TCA_SPLIT_LCMP2EN_bm;	//Enable low compare bits
	TCA0.SPLIT.CTRLB |= TCA_SPLIT_HCMP0EN_bm | TCA_SPLIT_HCMP1EN_bm | TCA_SPLIT_HCMP2EN_bm; //Enable High compare bits
}

void TCA1_init(){
	PORTB.DIR |= (1<<3);
	PORTB.PINCONFIG |= (1<<3);
	PORTMUX.TCAROUTEA |= PORTMUX_TCA1_PORTB_gc; //Setup
	TCA1.SPLIT.CTRLD = TCA_SPLIT_SPLITM_bm;	//Enable splitmode
	TCA1.SPLIT.HPER = 0xFF; //Set period Lowbits
	TCA1.SPLIT.LPER = 0xFF; //Set period Highbits
	TCA1.SPLIT.CTRLA = TCA_SPLIT_ENABLE_bm | TCA_SPLIT_CLKSEL_DIV16_gc; //Enable clock and set prescaler (crystal clk/16)
	TCA1.SPLIT.CTRLB = TCA_SPLIT_LCMP2EN_bm;	//Enable low compare bits
	TCA1.SPLIT.CTRLB |= TCA_SPLIT_HCMP0EN_bm | TCA_SPLIT_HCMP1EN_bm | TCA_SPLIT_HCMP2EN_bm; //Enable High compare bits
}

void TCB0_init(){
	PORTC.DIRCLR = (1<<0);
	TCB0.CCMP = 0x00;
	TCB0.CNT = 0x00;
	TCB0.CTRLA |= TCB_ENABLE_bm | TCB_CLKSEL_DIV1_gc;
	TCB0.CTRLB = (1<<TCB_CCMPEN_bp) | TCB_CNTMODE_FRQ_gc;
	TCB0.EVCTRL |= TCB_CAPTEI_bm;
	
}
