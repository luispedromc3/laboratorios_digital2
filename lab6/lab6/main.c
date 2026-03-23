/*
 * lab6.c
 *
 * Created: 3/17/2023
 * Author: Luis Pedro Monterroso
 * Description: 
 */
/****************************************/
// Encabezado (Libraries)
#include <avr/io.h>
#include <stdint.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include "UART/UART.h"
/****************************************/
// Function prototypes
void setup();
/****************************************/
// Main Function
int main(void)
{
	cli();
	setup();
	initUART();
	sei();
	while (1)
	{
	}
}
/****************************************/
// NON-Interrupt subroutines
void setup(){
	//entradas
	DDRC &= ~( (1 << PORTC0)|(1 << PORTC1)|(1 << PORTC3)|(1 << PORTC4)|(1 << PORTC5)); 
	DDRB &= ~( (1 << PORTB5));
	// salidas
	DDRB |= (1 << PORTB3);
	PORTB &= ~( (1 << PORTB3));
	//pullups activos
	PORTC |= (1 << PORTC0)|(1 << PORTC1)|(1 << PORTC3)|(1 << PORTC4)|(1 << PORTC5);
	PORTB |= (1 << PORTB4);
	
	// Habilitar interrupciones Pin Change para PORTB y PORTC
	PCICR |= (1 << PCIE0) | (1 << PCIE1);
	// Habilitar interrupción en PB4 (PCINT4)
	PCMSK0 |= (1 << PCINT4);
	// habilitiar resto de interrupciones en PC
	PCMSK1 |= (1 << PCINT8) |(1 << PCINT9) |(1 << PCINT11) |(1 << PCINT12) |(1 << PCINT13);
	
};
/****************************************/
// Interrupt routines
ISR(PCINT0_vect) {
	
	if (!(PINB & (1 << PB4))) {
		// PB5 presionado (LOW)
		PORTB ^= (1 << PB3);
		writeString("I");
	}
}
ISR(PCINT1_vect) {

	if (!(PINC & (1 << PC0))) {
		// PC0 presionado
		PORTB ^= (1 << PB3);
		writeString("D");
	} else if (!(PINC & (1 << PC1))) {
		// PC1 presionado
		PORTB ^= (1 << PB3);
		writeString("A");
	}else if (!(PINC & (1 << PC3))) {
		// PC3 presionado
		PORTB ^= (1 << PB3);
		writeString("R");
	}else if (!(PINC & (1 << PC4))) {
		// PC4 presionado
		PORTB ^= (1 << PB3);
		writeString("a");
	}else if (!(PINC & (1 << PC5))) {
		// PC5 presionado
		PORTB ^= (1 << PB3);
		writeString("b");
	}
}






