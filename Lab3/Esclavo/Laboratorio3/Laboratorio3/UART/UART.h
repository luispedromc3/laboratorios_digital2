/*
 * UART.h
 *
 * Created: 2/3/2026 10:11:44 AM
 *  Author: luisp
 */ 


#ifndef UART_H_
#define UART_H_
#define F_CPU 16000000
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h> // para itoa()

void initUART();
void WriteCharUART(char c);
void WriteString(char* string);
void writeNumber(uint8_t number);
#endif /* UART_H_ */