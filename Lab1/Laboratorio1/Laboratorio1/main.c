/*
 * Laboratorio1.c
 *
 * Created: 1/20/2026
 * Author: Luis Pedro Monterroso
 * Description: 
 */
/****************************************/
// Encabezado (Libraries)
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
uint8_t contador_pb;
volatile uint16_t contador_timer;
uint16_t contador_display;
uint8_t bandera_display;
uint8_t bandera_start;
uint8_t contador_amarillo;
uint8_t contador_rojo;
uint8_t contador_pb5;
/****************************************/
// Function prototypes
void setup();
/****************************************/
// Main Function
int main(void)
{
	setup();
	/* Replace with your application code */
	while (1)
	{
	}
}
/****************************************/
// NON-Interrupt subroutines
void setup(){
	cli();
	UCSR0B = 0;  
	//Definir entradas y salidas (DDR = 1 salida , DDR = 0 entrada)
	DDRC = 0;
	DDRC |= (1 << PORTC0)|(1 << PORTC1)|(1 << PORTC2)|(1 << PORTC3); // activo las salidas 
	PORTC = 0XFF; // activo pullups
	PORTC &= ~( (1 << PORTC0)|(1 << PORTC1)|(1 << PORTC2)|(1 << PORTC3)); // salidas inicialmente apagadas
	
	DDRD = 0xFF;
	PORTD = 0;
	
	//INTERRUPCION DE PINCHANGE
	// Habilitar interrupciones Pin Change para PORTC
	PCICR |= (1 << PCIE1);

	// Habilitar interrupción en PC4 y PC5 (PCINT12)
	PCMSK1 |= (1 << PCINT12)|(1 << PCINT13);
	 // PB4 como entrada y el resto salidas
	DDRB = 0xFF;
	DDRB &= ~(1 << PB4);

	// Pull-up interno y salidas apagadas
	PORTB = 0;
	PORTB |= (1 << PB4);
	// Habilitar grupo PCINT0
	PCICR |= (1 << PCIE0);
	// Habilitar PCINT5 (PB5)
	PCMSK0 |= (1 << PCINT4);
	//INTERRUPCION DE TIMER0
	// Modo Normal
	TCCR0A = 0;
	TCCR0B = 0;

	TCNT0 = 6; // Reiniciar contador

	// Prescaler = 64
	// Frecuencia del timer = 16 MHz / 64 = 250 kHz
	TCCR0B |= (1 << CS01) | (1 << CS00);

	// Habilitar interrupción por overflow
	TIMSK0 |= (1 << TOIE0);
	
	//inicializar banderas
	contador_pb = 0;
	contador_timer = 0;
	contador_display = 5;
	bandera_display = 0;
	bandera_start = 0;
	contador_amarillo = 0;
	contador_rojo = 0;
	contador_pb5 = 0;
	sei(); // Habilitar interrupciones globales
}
/****************************************/
// Interrupt routines
ISR(PCINT1_vect) {
	// Se ejecuta cuando PC4 cambia de estado (0?1 o 1?0)
	
	contador_pb++;
	if (contador_pb == 1){
		//PORTC ^= (1 << PORTC0);
		
		if (!(PINC & (1 << PORTC4))) { // Si PC4 es presionado (LOW)
			//PORTC ^= (1 << PORTC3);
			if (bandera_start == 1){
				contador_amarillo++;
				if(contador_amarillo <= 14){
					PORTC = (PORTC & 0xF0) | (contador_amarillo & 0x0F);
				}else{
					bandera_start = 0;
					// ganador 2
					PORTC = (PORTC & 0xF0) | (0xFF & 0x0F);
					PORTB = (PORTB & 0xF0) | (0x00 & 0x0F);
					PORTD = 0b11011100;
					bandera_display = 0;
					contador_display = 5;
					contador_amarillo = 0;
					contador_rojo = 0;
				}
				
			}
		}else if (!(PINC & (1 << PORTC5))) { // Si PC5 es presionado (LOW)
			//PORTC ^= (1 << PORTC2);
			if (bandera_start == 1){
				contador_rojo++;
				if(contador_rojo <= 14){
					PORTB = (PORTB & 0xF0) | (contador_rojo & 0x0F);
				}else{
					bandera_start = 0;
					// ganador 1
					PORTB = (PORTB & 0xF0) | (0xFF & 0x0F);
					PORTC = (PORTC & 0xF0) | (0x00 & 0x0F);
					PORTD = 0b1000001;
					bandera_display = 0;
					contador_display = 5;
					contador_amarillo = 0;
					contador_rojo = 0;
				}
				
			}
		}
	} else{
		contador_pb = 0;
	}
	
}
ISR(PCINT0_vect) {
	contador_pb5++;
	if (contador_pb5 == 1){
		if (!(PINB & (1 << PORTB4))) { // Si PB5 es presionado (LOW)
			//PORTC ^= (1 << PORTC0);
			bandera_display = 1;
			PORTB = (PORTB & 0xF0) | (contador_rojo & 0x0F);
			PORTC = (PORTC & 0xF0) | (contador_amarillo & 0x0F);
		}
	}else{
		contador_pb5 = 0;
	}
	
}
ISR(TIMER0_OVF_vect) {
	//PORTC ^= (1 << PORTC1);
	TCNT0 = 6;  // reduce el tiempo hasta el próximo overflow
	contador_timer++;
	if(contador_timer >= 1000){
		//PORTC ^= (1 << PORTC1);
		contador_timer = 0;
		if (bandera_display == 1){
			if (contador_display == 5){
				PORTD = 0b10111001;
				contador_display = contador_display - 1;
			} else if (contador_display == 4){
				PORTD = 0b1110001;
				contador_display = contador_display - 1;
			}else if (contador_display == 3){
				PORTD = 0b11011001;
				contador_display = contador_display - 1;
			}else if (contador_display == 2){
				PORTD = 0b11011100;
				contador_display = contador_display - 1;
			}else if (contador_display == 1){
				PORTD = 0b1000001;
				contador_display = contador_display - 1;
			}else if (contador_display == 0){
				PORTD = 0b11101101;
				bandera_start = 1;
			}
		}
	}
}






