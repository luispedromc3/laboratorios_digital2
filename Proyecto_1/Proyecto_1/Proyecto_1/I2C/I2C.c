/*
 * I2C.c
 *
 * Created: 2/10/2026 12:11:22 PM
 *  Author: luisp
 */ 
#define F_CPU 16000000
#include <avr/io.h>
#include <stdint.h>
void I2C_MasterInit(unsigned long SCL_Clock, uint8_t Prescaler){
	DDRC &= ~((1<<DDC4)|(1<<DDC5)); //entradas de I2C, SDA y SCL
	//colocamos prescaler
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
			TWSR0 |= ((1<<TWPS0)|(1<<TWPS1));
			break;
		default:
			TWSR0 &= ~((1<<TWPS1)|(1<<TWPS0)); 
			Prescaler = 1; 
	}
	TWBR0 = ((F_CPU/SCL_Clock)-16)/(2*Prescaler);
	TWCR0 |= (1<< TWEN);
	
}

uint8_t I2C_MasterStart(void){
	TWCR0 = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN); //Limpiar bandera de interrupcion, iniciar secuencia start y volver a habilitar interfaz.
	while(!(TWCR0 & (1<<TWINT))); // esperamos que se de la interrupcion
	
	return ((TWSR0 & 0xF8) == 0x08); // revisar los bits de estado
}
uint8_t I2C_MasterRepeatedStart(void){
	TWCR0 = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN); //Limpiar bandera de interrupcion, iniciar secuencia start y volver a habilitar interfaz.
	while(!(TWCR0 & (1<<TWINT))); // esperamos que se de la interrupcion
	
	return ((TWSR0 & 0xF8) == 0x10); // revisar los bits de estado
}
uint8_t I2C_MasterStop(void){
	TWCR0 = (1<<TWINT)|(1<<TWSTO)|(1<<TWEN); //iniciar secuencia stop
	while(TWCR0 & (1<<TWSTO)); // nos salimos cuando ya este.
}
uint8_t I2C_MasterWrite(uint8_t dato){
	uint8_t estado;
	TWDR0 = dato;
	TWCR0 = (1<<TWEN)|(1<<TWINT);
	while(!(TWCR0 & (1<<TWINT)));
	estado = TWSR0 & 0xF8; // me quedon con los bits de estado
	if(estado==0x18 || estado == 0x28){
		return 1;
	}else{
		return estado;
	}
}
uint8_t I2C_MasterRead(uint8_t *buffer, uint8_t ack){
	uint8_t estado;
	if(ack){
		TWCR0 = (1<<TWINT)|(1<<TWEA)|(1<<TWEN);
	} else{
		TWCR0 = (1<<TWINT)|(1<<TWEN);
	}
	while(!(TWCR0 & (1<<TWINT)));
	estado = TWSR0 & 0xF8;
	if(ack && estado != 0x50) return 0; // data recibida con ACK
	if(!ack && estado != 0x58) return 0; // data recibida sin ACK
	
	*buffer = TWDR0;
	return 1;
}
void I2C_SlaveInit(uint8_t address){
	DDRC &= ~((1<<DDC4)|(1<<DDC5));
	TWAR0 = address << 1;
	TWCR0 = (1<<TWEN)|(1<<TWEA)|(1<<TWIE);
}