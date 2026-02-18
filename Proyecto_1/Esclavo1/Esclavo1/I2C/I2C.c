/*
 * I2C.c
 *
 * Created: 2/9/2026 8:03:16 PM
 *  Author: admin
 */ 
 
#include "I2C.h" 
 
void I2C_Master_Init(unsigned long SCL_Clock, uint8_t Prescaler){
	DDRC &= ~((1 << DDC4) | (1 << DDC5)); // Los pines de i2C SDA y SCL deben ser entradas
	
	switch(Prescaler){
		case 1:
		TWSR &= ~((1<<TWPS1)|(1<<TWPS0));
		break;
		case 4:
		TWSR &= ~(1<<TWPS1);
		TWSR |= (1<<TWPS1);
		break;
		case 16:
		TWSR &= ~(1<<TWPS0);
		TWSR |= (1<<TWPS1);
		break;
		case 64:
		TWSR |= ((1<<TWPS1)|(1<<TWPS0));
		break;
		default:
		TWSR &= ~((1<<TWPS1)|(1<<TWPS0));
		Prescaler = 1;
		break;
	}
	
	TWBR = ((F_CPU/SCL_Clock)-16)/(2*Prescaler); // Calcular y asignar la velocidad
	TWCR |= (1 << TWEN); // Activar la interface (TWI) I2C
}

uint8_t I2C_Master_Start(void){
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	
	while (!(TWCR & (1 << TWINT))); // Espera a que la bander encienda
	return ((TWSR & 0xF8) == 0x08); // Nos interesa unicamente los bits de estado
}

uint8_t I2C_Master_RepeatedStart(void){
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	
	while (!(TWCR & (1 << TWINT))); // Espera a que la bander encienda
	
	return ((TWSR & 0xF8) == 0x10); // Nos interesa unicamente los bits de estado
}

void I2C_Master_stop(void){
	TWCR = (1 << TWEN) | (1 << TWINT) | (1 << TWSTO);
	while (TWCR & (1 << TWSTO)); // Esperamos a que se limpie el bit
	
}

uint8_t I2C_Master_Write(uint8_t dato){
	uint8_t estado;
	
	TWDR = dato; // Cargar el dato al registro para mandar al esclavo
	TWCR = (1 << TWEN) | (1 << TWINT); // Inicial la secuencia de envio
	
	while (!(TWCR & (1 << TWINT))); // Esperar a que se limpie la bandera 
	
	estado = TWSR & 0xF8;
	
	// Verificar si se transmitio una SLA + W con ACK o un dato con ACK
	if (estado == 0x18 || estado == 0x28){
		return 1;
	}else{
		return estado;
	}
		
}

uint8_t I2C_Master_Read(uint8_t *buffer, uint8_t ack){
	uint8_t estado;
	
	if(ack){
		// ACK, lo interpretamos como que quiero mas valores
		TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);	
	}else{	
		// NACK: ultimo byte
		TWCR = (1 << TWINT) | (1 << TWEN);
	}
	while (!(TWCR & (1 << TWINT))); // Esperamos el flag de la interrupcion
	estado = TWSR & 0xF8;
	
	// Verificar si se recicbio el dato con ACK o NACK
	if (ack && estado != 0x50) return 0;
	if (!ack && estado != 0x58) return 0;
	
	*buffer = TWDR;
	return 1;
}

void I2C_Slave_Init(uint8_t adress){
	DDRC &= ~((1 << DDC4) | (1 << DDC5)); // Los pines de i2C SDA y SCL deben ser entradas
	
	TWAR = adress << 1;
	
	TWCR = (1 << TWEA) | (1 << TWEN) | (1 << TWIE);	
}



