/*
 * UART.h
 *
 * Created: 4/30/2025 9:04:19 AM
 *  Author: admin
 */ 


#ifndef UART_H_
#define UART_H_

#include <avr/io.h>
#include <stdlib.h>

void initUART();
void writeChar(char caracter);
void writeString(char* texto);
void writeNumber8(uint8_t number);



#endif /* UART_H_ */