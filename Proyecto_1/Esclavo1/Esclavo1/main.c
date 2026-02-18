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
#include "PWM0/PWM0.h"

#define SlaveAdress 0x30
#define DHT_PIN     PORTC0
#define DHT_PORT    PORTC
#define DHT_DDR     DDRC
#define DHT_PINREG  PINC


// Variables
uint8_t buffer = 0;
uint8_t pwm_anterior = 0;
uint8_t pwm = 0;
volatile uint8_t rx_state = 0;
	
//volatile uint8_t valorADC = 0;

// Variables sensor
uint8_t data[5] = {0}; // Vector

// Prototyopes functions
void setup();
uint8_t DHT11_read(uint8_t *data);


// Mainloop
int main(void)
{
    setup();
	//data[2] = 255;
    while (1) 
    {
		//updateDutyCycleA0(195);
		if (buffer == 'R'){
			if (DHT11_read(data))
			{
				// lectura correcta
			}
			buffer = 0;
		}
		
		
		updateDutyCycleA0(pwm);
		
		
		
		
    }
}

// NON-interrupts subroutines
void setup(){
	cli();
	
	//initADC();
	I2C_Slave_Init(SlaveAdress);
	initPWMFastA0(no_invert, 8);
	
	PORTB |= (1 << PORTB5);   // Encender LED (HIGH)
	PORTB &= ~(1 << PORTB5);  // Apagar LED (LOW)

	DDRC |= (1<<PORTC1) | (1<<PORTC2);   // IN1 e IN2 como salida

	PORTC |= (1<<PORTC1);   // IN1 = 1
	PORTC &= ~(1<<PORTC2);  // IN2 = 0
	sei();
	
}

uint8_t DHT11_read(uint8_t *data)
{
	uint16_t timeout;
	uint8_t mask = 0x80;
	uint8_t idx = 0;

	// Limpiar buffer
	for (uint8_t i = 0; i < 5; i++)
	data[i] = 0;

	cli();  // ?? IMPORTANTE: sin interrupciones

	// Se?al de inicio
	DHT_DDR |= (1 << DHT_PIN);     // salida
	DHT_PORT &= ~(1 << DHT_PIN);   // LOW
	_delay_ms(20);

	DHT_PORT |= (1 << DHT_PIN);    // HIGH
	_delay_us(30);
	DHT_DDR &= ~(1 << DHT_PIN);    // entrada
	DHT_PORT |= (1 << DHT_PIN);    // pull-up

	// Esperar respuesta LOW
	timeout = 20000;
	while ((DHT_PINREG & (1 << DHT_PIN)) && --timeout);
	if (!timeout) { sei(); return 0; }

	// Esperar respuesta HIGH
	timeout = 20000;
	while (!(DHT_PINREG & (1 << DHT_PIN)) && --timeout);
	if (!timeout) { sei(); return 0; }

	// Esperar LOW inicial
	timeout = 20000;
	while ((DHT_PINREG & (1 << DHT_PIN)) && --timeout);
	if (!timeout) { sei(); return 0; }

	// Leer 40 bits
	for (uint8_t i = 0; i < 40; i++)
	{
		// Esperar HIGH
		timeout = 20000;
		while (!(DHT_PINREG & (1 << DHT_PIN)) && --timeout);
		if (!timeout) { sei(); return 0; }

		_delay_us(30);  // ?? punto cr?tico

		if (DHT_PINREG & (1 << DHT_PIN))
		data[idx] |= mask;

		mask >>= 1;
		if (!mask)
		{
			mask = 0x80;
			idx++;
		}

		// Esperar LOW
		timeout = 20000;
		while ((DHT_PINREG & (1 << DHT_PIN)) && --timeout);
		if (!timeout) { sei(); return 0; }
	}

	sei();  // ?? reactivar interrupciones

	// Verificar checksum
	uint8_t sum = data[0] + data[1] + data[2] + data[3];
	if (sum != data[4])
	return 0;

	return 1;
}




// Interrupts 


ISR(TWI_vect){
	uint8_t estado = TWSR & 0xF8; // OJO cambie 0xFC por 0xF8
	//PORTB ^= (1 << PORTB5);
	switch(estado){
		// Slave debe de recibir un dato
		case 0x60: // SLA+W recibido
		case 0x70: // General Call
			TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWIE) | (1 << TWEA);
			break;
			
		case 0x80: // Dato recibido, ACK enviado
		case 0x90: // Dato recibido General Call, ACK enviado
			 if(rx_state == 0)
			 {
				 buffer = TWDR;

				 if(buffer == 'V')
				 rx_state = 1;   // esperamos PWM
			 }
			 else if(rx_state == 1)
			 {
				  
				 pwm = TWDR;       // Guardamos PWM
				 rx_state = 0;
			 }
			TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWIE) | (1 << TWEA);
			break;
			
		// Slave de transmitir un dato
		case 0xA8: // SLA+R recibido
		case 0xB8: // Dato transmitido, ACK recibido
			
			TWDR = data[2]; // Dato a enviar 
			TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWIE) | (1 << TWEA);
			break;
		// Esto me sirve solo para mandar 3 bytes
		// Podriamos hacer una variable de indice, para saber que valor enviar
		
		case 0xC0: // Dato transmitido, NACK
		case 0xC8: // Ultimo dato transimitdo
		
			TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWIE) | (1 << TWEA);
			break;
		
		case 0xA0: // STOP o repeated START recibido como slave
			TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWIE) | (1 << TWEA);
			break;
		
		default:
			TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWIE) | (1 << TWEA);
			
			break;	
	}
	
	
}