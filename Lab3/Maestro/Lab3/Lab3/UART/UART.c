/*
 * UART.c
 *
 * Created: 4/30/2025 9:04:08 AM
 *  Author: admin
 */ 

#include "UART.h"

void initUART(){
	// Paso 1: estableceer PD1 (TX) como salida y PD0 como entrada (RX)
	DDRD &= ~(1 << DDD0); // PD0 como entrada (RX)
	DDRD |= (1 << DDD1);  // PD1 como salida (TX)

	
	// Paso 2: Configurar UCSR0A
	UCSR0A = 0;
	UCSR0A |= (1 << U2X0); // Doble velocidad
	
	// Paso 3: Configurar UCSR0B
	UCSR0B = (1 << RXCIE0) | (1 << RXEN0) | (1 << TXEN0);
	
	// Paso 4: Configurar UCSR0C
	// Configurar UART como asincrono, sin polaridad, 1 stop bit y 8 bits de datos
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
	
	// Paso 5: Configurar baudrate UBR0 = 12 -> 9600 @ 1 MHz a doble velocidad
	UBRR0 = 12;
}

void writeChar(char caracter){
	while(( UCSR0A & (1 << UDRE0)) == 0){
	}
	UDR0 = caracter;
}

void writeString(char* texto){
	for(uint8_t i = 0; *(texto+i) != '\0' ; i++){
		writeChar(*(texto+i));
	}
}

void writeNumber8(uint8_t number){
	char buffer[4]; // Máx: "255\0"
	itoa(number, buffer, 10); // Convierte número a string en base 10
	writeString(buffer);
}