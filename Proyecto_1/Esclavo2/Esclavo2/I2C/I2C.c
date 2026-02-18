/*
 * I2C.c
 *
 * Created: 2/9/2026 8:03:16 PM
 *  Author: admin
 */ 
 
#include "I2C.h"


void I2C_Master_Init(unsigned long SCL_Clock, uint8_t Prescaler){
	DDRC &= ~((1 << DDC4) | (1 << DDC5));
	
	switch(Prescaler){
		case 1:
		TWSR0 &= ~((1<<TWPS1)|(1<<TWPS0));
		break;
		
		case 4:
		TWSR0 &= ~(1<<TWPS1);
		TWSR0 |= (1<<TWPS0);
		break;
		
		case 16:
		TWSR0 &= ~(1<<TWPS0);
		TWSR0 |= (1<<TWPS1);
		break;
		
		case 64:
		TWSR0 |= ((1<<TWPS1)|(1<<TWPS0));
		break;
		
		default:
		TWSR0 &= ~((1<<TWPS1)|(1<<TWPS0));
		Prescaler = 1;
		break;
	}
	
	TWBR0 = ((F_CPU/SCL_Clock)-16)/(2*Prescaler);
	TWCR0 |= (1 << TWEN);
}

uint8_t I2C_Master_Start(void){
	TWCR0 = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	
	while (!(TWCR0 & (1 << TWINT)));
	
	return ((TWSR0 & 0xF8) == 0x08);
}

uint8_t I2C_Master_RepeatedStart(void){
	TWCR0 = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	
	while (!(TWCR0 & (1 << TWINT)));
	
	return ((TWSR0 & 0xF8) == 0x10);
}

void I2C_Master_Stop(void){
	TWCR0 = (1 << TWEN) | (1 << TWINT) | (1 << TWSTO);
	while (TWCR0 & (1 << TWSTO));
}

uint8_t I2C_Master_Write(uint8_t dato){
	uint8_t estado;
	
	TWDR0 = dato;
	TWCR0 = (1 << TWEN) | (1 << TWINT);
	
	while (!(TWCR0 & (1 << TWINT)));
	
	estado = TWSR0 & 0xF8;
	
	if (estado == 0x18 || estado == 0x28){
		return 1;
		}else{
		return estado;
	}
}

uint8_t I2C_Master_Read(uint8_t *buffer, uint8_t ack){
	uint8_t estado;
	
	if(ack){
		TWCR0 = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
		}else{
		TWCR0 = (1 << TWINT) | (1 << TWEN);
	}
	
	while (!(TWCR0 & (1 << TWINT)));
	
	estado = TWSR0 & 0xF8;
	
	if (ack && estado != 0x50) return 0;
	if (!ack && estado != 0x58) return 0;
	
	*buffer = TWDR0;
	return 1;
}

void I2C_Slave_Init(uint8_t address){
	DDRC &= ~((1 << DDC4) | (1 << DDC5));
	
	TWAR0 = address << 1;
	
	TWCR0 = (1 << TWEA) | (1 << TWEN) | (1 << TWIE);
}
