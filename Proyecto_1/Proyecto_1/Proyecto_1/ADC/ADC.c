/*
 * ADC.c
 *
 * Created: 2/3/2026 12:54:19 PM
 *  Author: luisp
 */ 
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h> // para itoa()
#include <stdbool.h>
#include <stdlib.h>

void initADC(void){
	ADMUX = 0;
	DDRC = 0;
	PORTC = 0;
	ADMUX |= (1 << REFS0) ; //VCC COMO COMPARADOR
	ADMUX |= (1 << ADLAR) ; //JUSTIFICADO A LA IZQUIERDA USAR HIGH
	ADMUX |= (1 << MUX2)|(1 << MUX1); //ADC6
	ADCSRA = 0;
	ADCSRA |= (1 << ADPS1) | (1 << ADPS0)| (1 << ADPS2); //PRESCALER DE 128
	ADCSRA |= (1 << ADEN);
	ADCSRA |= (1 << ADIE); //ACTIVAR INTERRUPCIONES
	ADCSRA |= (1 << ADSC); //EMPEZAR EL ADC
}