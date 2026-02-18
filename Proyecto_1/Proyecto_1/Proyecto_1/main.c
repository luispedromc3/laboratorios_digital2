/*
 * Esclavo1.c
 *
 * Created: 2/9/2026 10:32:55 PM
 * Author : admin
 */ 

// Librerias
#define F_CPU 16000000

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "I2C/I2C.h"
#include "ADC/ADC.h"

#define SlaveAdress 0x40

// Variables
uint8_t buffer = 0;
volatile uint8_t valorADC = 0;

// Variables sensor
uint8_t data[5] = {0}; // Vector

// Prototyopes functions
void setup();

void pedirTemp();
void esperarRespuestaTemp();
uint8_t dht_read_bit(void);
void leerBits();

// Mainloop
int main(void)
{
    setup();
	data[2] = 255;
    while (1) 
    {
		if (buffer == 'R'){
			cli();
			pedirTemp();
			esperarRespuestaTemp();
			leerBits();
			sei();
			buffer = 0;
		}
		
		
	
	ADCSRA |= (1 << ADSC);
	
	
	
	_delay_ms(250);
		
    }
}

// NON-interrupts subroutines
void setup(){
	cli();
	
	initADC();
	I2C_SlaveInit(SlaveAdress);
	
	 PORTB |= (1 << PORTB5);   // Encender LED (HIGH)
	 PORTB &= ~(1 << PORTB5);  // Apagar LED (LOW)

	sei();
	
}

void pedirTemp(){
	// PD2 como salida
	DDRD |= (1 << 2);

	// LOW ? 18 ms
	PORTD &= ~(1 << 2);
	_delay_ms(18);

	// HIGH 20–40 us
	PORTD |= (1 << PORTD2);
	_delay_us(30);

	// Liberar línea ? entrada
	DDRD &= ~(1 << PORTD2);
}

void esperarRespuestaTemp(){
	// Espera LOW (~80 us)
	while (PIND & (1 << PORTD2));

	// Espera HIGH (~80 us)
	while (!(PIND & (1 << PORTD2)));

	// Espera LOW (fin de respuesta)
	while (PIND & (1 << PORTD2));
}

uint8_t dht_read_bit(void) {
	// Espera HIGH (inicio del bit)
	while (!(PIND & (1 << PORTD2)));

	_delay_us(40);

	// Si sigue alto ? 1, si ya bajó ? 0
	if (PIND & (1 << PORTD2))
	return 1;
	else
	return 0;
}

void leerBits(){
	
	for (uint8_t i = 0; i < 5; i++) {
		data[i] = 0;
	} // Limpiar registro
	for (uint8_t i = 0; i < 40; i++) {
		data[i / 8] <<= 1;
		data[i / 8] |= dht_read_bit();
		
	}
	
}

// Interrupts 
ISR(ADC_vect){
	valorADC = ADCH;
	
}
ISR(TWI0_vect){
	uint8_t estado = TWSR0 & 0xF8; // OJO cambie 0xFC por 0xF8
	PORTB ^= (1 << PORTB5);
	switch(estado){
		// Slave debe de recibir un dato
		case 0x60: // SLA+W recibido
		case 0x70: // General Call
			TWCR0 = (1 << TWINT) | (1 << TWEN) | (1 << TWIE) | (1 << TWEA);
			break;
			
		case 0x80: // Dato recibido, ACK enviado
		case 0x90: // Dato recibido General Call, ACK enviado
			buffer = TWDR0;
			TWCR0 = (1 << TWINT) | (1 << TWEN) | (1 << TWIE) | (1 << TWEA);
			break;
			
		// Slave de transmitir un dato
		case 0xA8: // SLA+R recibido
		case 0xB8: // Dato transmitido, ACK recibido
			
			TWDR0 = data[2]; // Dato a enviar 
			TWCR0 = (1 << TWINT) | (1 << TWEN) | (1 << TWIE) | (1 << TWEA);
			break;
		// Esto me sirve solo para mandar 3 bytes
		// Podriamos hacer una variable de indice, para saber que valor enviar
		
		case 0xC0: // Dato transmitido, NACK
		case 0xC8: // Ultimo dato transimitdo
		
			TWCR0 = (1 << TWINT) | (1 << TWEN) | (1 << TWIE) | (1 << TWEA);
			break;
		
		case 0xA0: // STOP o repeated START recibido como slave
			TWCR0 = (1 << TWINT) | (1 << TWEN) | (1 << TWIE) | (1 << TWEA);
			break;
		
		default:
			TWCR0 = (1 << TWINT) | (1 << TWEN) | (1 << TWIE) | (1 << TWEA);
			
			break;	
	}
	
	
}