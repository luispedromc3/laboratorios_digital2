/*
 * UART.c
 *
 * Created: 2/3/2026 10:11:54 AM
 *  Author: luisp
 */ 
#define F_CPU 16000000
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h> // para itoa()
#include "UART.h"
void initUART(){
	//SALIDA Y ENTRADA DEL RX Y TX
	DDRD |= (1 << DDD1);
	DDRD &= ~(1 << DDD0);
	//conf de banderas
	UCSR0A = 0;
	//configurar UCSR0B
	UCSR0B |= (1 << RXCIE0)|(1 << RXEN0)|(1 << TXEN0);
	//8 bits de datos, sin polaridad, 1 STOP BIT y asincrono
	UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00);
	//configurar BAUDRATE, es decir velocidad de transmision. UBRR0 = 103 -> 9600 @ 16MHz
	UBRR0 = 103;
}

void WriteCharUART(char c){
	while((UCSR0A & (1 << UDRE0)) == 0);
	UDR0 = c;
}
void WriteString(char* string){
	for (uint16_t i=0; *(string+i) != '\0' ; i++){
		WriteCharUART(*(string+i));
	}
}
void writeNumber(uint8_t number) {
	char buffer[4]; // Máx: "255\0"
	itoa(number, buffer, 10); // Convierte número a string en base 10
	WriteString(buffer);
}
