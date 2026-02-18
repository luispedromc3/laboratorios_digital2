/*
 * Esclavo2.c
 *
 * Created: 2/11/2026 12:27:05 PM
 * Author : admin
 */ 

// Librerias
#define F_CPU 16000000

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "I2C/I2C.h"
#include "ADC/ADC.h"
#include "HX711/HX711.h"
#include "PWM1/PWM1.h"

#define SlaveAdress 0x40

// Variables
uint8_t buffer = 0;
volatile uint16_t peso = 0;
volatile uint8_t i2c_index = 0;

volatile uint16_t pasos_recibidos = 0;
volatile uint8_t rx_state = 0;

volatile int16_t posicion_actual = 0;
int16_t posicion_deseada = 0;
int16_t diferencia = 0;

volatile int16_t posicion_objetivo = 0;
uint8_t fase_actual = 0;

uint8_t temp_flag = 0;
// Stepper
uint8_t stepSequence[4] = {
	0b0011,
	0b0110,
	0b1100,
	0b1001
};

// Prototipos
void setup();
void Stepper_Move(int16_t pasos);
void Servo_SetAngle(uint8_t angle);

int main(void)
{
	setup();
	while (1)
	{
		if (buffer == 'R'){
			peso = HX711_ReadAverage(5); // preparamos valor del peso
			buffer = 0;
		}
		
		if (buffer == 'S'){
			
			buffer = 0;
		}
	
		// Movimiento del stepper 
		if(posicion_actual != posicion_objetivo)
		{
			if(posicion_actual < posicion_objetivo)
			{
				fase_actual = (fase_actual + 1) % 4; // MOD4
				posicion_actual++;
			}
			else
			{
				fase_actual = (fase_actual + 3) % 4; // retroceso MOD4
				posicion_actual--;
			}
			
			if(posicion_actual > 512) posicion_actual = 512;
			if(posicion_actual < 0) posicion_actual = 0;
			// Para siempte moverse en un rango de 0 a 90 grados
			
			PORTD &= 0b11000011; // Resetear las leds
			PORTD |= (stepSequence[fase_actual] << 2); // << Para estar en PD2 - PD5

			_delay_ms(2);
		}
		
		// Movimineto del servo
		if (temp_flag){
			PORTB |= (1 << PORTB5);
			Servo_SetAngle(0);
		}else{
			PORTB &= ~(1 << PORTB5);
			Servo_SetAngle(180);
		}
		
	}
	
}

void setup(){
	cli();
	
	I2C_Slave_Init(SlaveAdress);
	
	 HX711_Init();      // <-- IMPORTANTE
	_delay_ms(500);	
	 HX711_Tare();      // <-- Calibrar en cero (sin peso)
	
	initPWMFastA1(no_invert, 8, 40000);
	DDRD |= (1<<PORTD2)|(1<<PORTD3)|(1<<PORTD4)|(1<<PORTD5);
	PORTD &= ~((1<<PORTD2)|(1<<PORTD3)|(1<<PORTD4)|(1<<PORTD5));

	DDRB |= (1 << DDB5);  
	PORTB &= ~(1 << PORTB5);

	sei();
}

void Servo_SetAngle(uint8_t angle){
	uint16_t pulse = 2000 + ((uint32_t)angle * 2000) / 180;
	updateDutyCycleA1(pulse);
}



// =========================
// INTERRUPCION TWI0
// =========================

ISR(TWI0_vect){

	uint8_t estado = TWSR0 & 0xF8;
	//PORTB ^= (1 << PORTB5);

	switch(estado){

		// SLA+W recibido
		case 0x60:
		case 0x70:
		TWCR0 = (1 << TWINT) | (1 << TWEN) | (1 << TWIE) | (1 << TWEA);
		break;
		
		// Dato recibido
		case 0x80:
		case 0x90:
		switch(rx_state)
		{
			case 0:  // esperando comando
			{
				buffer = TWDR0;
				
				if(buffer == 'S') rx_state = 1;

				else if(buffer == 'P')rx_state = 4;

				break;
			}

			case 1:  // MSB pasos
			pasos_recibidos = ((uint16_t)TWDR0 << 8);
			rx_state = 2;
			break;

			case 2:  // LSB pasos
			pasos_recibidos |= TWDR0;
			posicion_objetivo = pasos_recibidos;
			rx_state = 0;  // listo para siguiente comando
			break;

			case 4:  // flag temperatura
			temp_flag = TWDR0;
			rx_state = 0;
			break;
		}

		TWCR0 = (1 << TWINT) | (1 << TWEN) | (1 << TWIE) | (1 << TWEA);
		break;
		
		// SLA+R recibido
		case 0xA8:
			i2c_index = 0;
		case 0xB8:
		 if(i2c_index == 0){
			 TWDR0 = (peso >> 8);   // MSB
			 i2c_index = 1;
		 }
		 else{
			 TWDR0 = (peso & 0xFF); // LSB
			 i2c_index = 0;
		 }

		 TWCR0 = (1 << TWINT) | (1 << TWEN) | (1 << TWIE) | (1 << TWEA);
		 break;
		
		case 0xC0:
		case 0xC8:
		case 0xA0:
		default:
		i2c_index = 0; // reset
		TWCR0 = (1 << TWINT) | (1 << TWEN) | (1 << TWIE) | (1 << TWEA);
		break;
	}
}

