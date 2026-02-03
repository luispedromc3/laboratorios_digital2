/*
 * LCD_8.c
 *
 * Created: 1/27/2026 10:07:13 AM
 *  Author: luisp
 */ 
#include "LCD_8.h"
//#define F_CPU 16000000
void initLC8bits(void){
	// PD2 a PD7 como salida
	DDRD |= (1 << PD2) | (1 << PD3) | (1 << PD4) |(1 << PD5) | (1 << PD6) | (1 << PD7);
	// PB0 a PB5 como salida
	DDRB |= (1 << PB0) | (1 << PB1) | (1 << PB2) |(1 << PB3) | (1 << PB4) | (1 << PB5);
	// PC0 como salida
	DDRC |= (1 << PC0);
	//salidas inicialmente apagadas
	PORTD &= ~((1 << PD2) | (1 << PD3) | (1 << PD4) |(1 << PD5) | (1 << PD6) | (1 << PD7));
	PORTB &= ~((1 << PB0) | (1 << PB1) | (1 << PB2) |(1 << PB3) | (1 << PB4) | (1 << PB5));
	PORTC &= ~(1 << PC0);
	LCD_port(0x00); //limpiamos el port
	_delay_ms(20); //esperamos que se inicialice 
	LCD_CMD(0x30); 
	_delay_ms(5); // seguimos algoritmo
	LCD_CMD(0x30);
	_delay_ms(5); // seguimos algoritmo
	LCD_CMD(0x30);
	_delay_ms(5); // seguimos algoritmo
	
	LCD_CMD(0x38);
	_delay_ms(1);
	
	LCD_CMD(0x08);
	_delay_ms(1);
	
	LCD_CMD(0x01);
	_delay_ms(3);
	
	LCD_CMD(0x06);
	_delay_ms(1);
	LCD_CMD(0x0C);

	_delay_ms(1);
}
void LCD_CMD(char a){
	//RS = 0
	PORTD &= ~(1<<PORTD2);
	//coloco el comando que quiero mandar
	LCD_port(a);
	// ahora coloco el EN en 1
	PORTD |= (1<<PORTD4);
	_delay_ms(4);
	//EN en 0
	PORTD &= ~(1<<PORTD4);
}
void LCD_port(char a){
	// Bit 0
	if (a & (1 << 0)) PORTD |= (1 << PD5);
	else                PORTD &= ~(1 << PD5);

	// Bit 1
	if (a & (1 << 1)) PORTD |= (1 << PD6);
	else                PORTD &= ~(1 << PD6);

	// Bit 2
	if (a & (1 << 2)) PORTD |= (1 << PD7);
	else                PORTD &= ~(1 << PD7);

	// Bit 3
	if (a & (1 << 3)) PORTB |= (1 << PB0);
	else                PORTB &= ~(1 << PB0);

	// Bit 4
	if (a & (1 << 4)) PORTB |= (1 << PB1);
	else                PORTB &= ~(1 << PB1);

	// Bit 5
	if (a & (1 << 5)) PORTB |= (1 << PB2);
	else                PORTB &= ~(1 << PB2);

	// Bit 6
	if (a & (1 << 6)) PORTB |= (1 << PB3);
	else                PORTB &= ~(1 << PB3);

	// Bit 7
	if (a & (1 << 7)) PORTB |= (1 << PB4);
	else                PORTB &= ~(1 << PB4);
}

void LCD_WriteChar(char a){
	//Ponemos RS en 1 para poder mandar un dato
	PORTD |= (1<<PORTD2);
	LCD_port(a);
	//Hacemos la transicion del enable
	PORTD |= (1<<PORTD4);
	_delay_ms(4);
	PORTD &= ~(1<<PORTD4);
}
void LCD_WriteString(char *a){
	int i;
	for(i=0; a[i] != '\0'; i++)
	LCD_WriteChar(a[i]);
}
void LCD_SetCursor(char c, char f){
	char temp = 0x80;
	if(f == 1){
		temp = 0x80 + c - 1;
		LCD_CMD(temp);
	}else if(f == 2){
		temp = 0xC0 + c - 1;
		LCD_CMD(temp);
	}else{
		return;
	}
}
//desplazamiento a la derecha
void LCD_ShiftRight(void){
	LCD_CMD(0X1C);
}
//desplazamiento a la izquierda
void LCD_ShiftLeft(void){
	LCD_CMD(0X18);
}