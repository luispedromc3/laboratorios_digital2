/*
 * PWM2.c
 *
 * Created: 4/30/2025 8:57:43 AM
 *  Author: admin
 */ 

#include "PWM0.h"

void initPWMFastA0(uint8_t invertido, uint16_t prescaler){
	DDRD |= (1 << DDD6);
	
	TCCR0A &= ~((1 << COM0A1) | (1 << COM0A0));
	
	if (invertido){
		TCCR0A |= (1 << COM0A1) | (1 << COM0A0);
		}else{
		TCCR0A |= (1 << COM0A1);
	}
	
	
	TCCR0A |= (1 << WGM01) | (1 << WGM00); // MODO FAST PWM
	
	TCCR0B = 0;
	switch(prescaler){
		case 1:
		TCCR0B |= (1 << CS00);
		break;
		case 8:
		TCCR0B |= (1 << CS01);
		break;
		case 64:
		TCCR0B |= (1 << CS01) | (1 << CS00);
		break;
		case 256:
		TCCR0B |= (1 << CS02);
		break;
		case 1024:
		TCCR0B |= (1 << CS02) | (1 << CS00);
		break;
		default:
		TCCR0B |= (1 << CS00);
		break;
	}

}

void initPWMFastB0(uint8_t invertido, uint16_t prescaler){
	DDRD |= (1 << DDD5);
	
	TCCR0A &= ~((1 << COM0B1) | (1 << COM0B0));
	
	if (invertido == invert){
		TCCR0A |= (1 << COM0B1) | (1 << COM0B0);
		}else{
		TCCR0A |= (1 << COM0B1);
	}
	
	TCCR0A |= (1 << WGM01) | (1 << WGM00); // MODO FAST PWM
	
	TCCR0B = 0;
	switch(prescaler){
		case 1:
		TCCR0B |= (1 << CS00);
		break;
		case 8:
		TCCR0B |= (1 << CS01);
		break;
		case 64:
		TCCR0B |= (1 << CS01) | (1 << CS00);
		break;
		case 256:
		TCCR0B |= (1 << CS02);
		break;
		case 1024:
		TCCR0B |= (1 << CS02) | (1 << CS00);
		break;
		default:
		TCCR0B |= (1 << CS00);
		break;
	}
}


void updateDutyCycleA0(uint8_t dutycycle){
	OCR0A = dutycycle;
}

void updateDutyCycleB0(uint8_t dutycycle){
	OCR0B = dutycycle;
}