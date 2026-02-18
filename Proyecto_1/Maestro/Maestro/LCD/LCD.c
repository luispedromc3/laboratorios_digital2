/*
 * CFile1.c
 *
 * Created: 1/27/2026 10:24:13 AM
 *  Author: admin
 */ 

#include "LCD.h"


void init_LCD8(){
	// Inicializar pines de salida para el LCD
	// PORTD 2-7
	DDRD |= (1<<PORTD2) | (1<<PORTD3) | (1<<PORTD4) | (1<<PORTD5) | (1<<PORTD6) | (1<<PORTD7);
	PORTD &= ~((1<<PORTD2) | (1<<PORTD3) | (1<<PORTD4) | (1<<PORTD5) | (1<<PORTD6) | (1<<PORTD7));
	// PORTB 0-1
	DDRB |= (1<<PORTB0) | (1 << PORTB1);
	PORTB &= ~((1<<PORTB0) | (1 << PORTB1));
	
	//Incializar pines de l?gica PORTC
	DDRC |= (1 << PORTC0) | (1 << PORTC1) | (1 << PORTC2);
	PORTC &= ~((1 << PORTC0) | (1 << PORTC1) | (1 << PORTC2));
	
	// Empezamos a configurar la pantalla
	LCD_Port8(0x00);
	_delay_ms(20);
	
	LCD_CMD8(0x30);
	_delay_ms(5);
	LCD_CMD8(0x30);
	_delay_ms(1);
	LCD_CMD8(0x30);
	_delay_ms(5);
	// Establecer funciones
	LCD_CMD8(0x38);
	LCD_CMD8(0x08);
	LCD_CMD8(0x01);
	LCD_CMD8(0x06);
	LCD_CMD8(0X0C);
	
}

void LCD_Port8(char a){
	uint8_t puerto_d = (a & 0b00111111) << 2;
	uint8_t puerto_b = (a & 0b11000000) >> 6; // M?scara para conservar ?nicamente los ?ltimos 2 bits
	
	PORTD = (PORTD & 0b00000011) | puerto_d;
	PORTB = (PORTB & 0b11111100) | puerto_b;
	
}

void LCD_CMD8(char a){
	// Poner el pin RS en cero para que la pantalla detecte los comandos
	PORTC &= ~(1 << PORTC0);
	
	// Escribir en el puerto
	LCD_Port8(a);
	// Encender EN
	PORTC |= (1 << PORTC2);
	_delay_ms(1);
	// Descativamos EN para que se termine de ejecutar la acci?n
	PORTC &= ~(1 << PORTC2);
}

void LCD_Write_Char8(char c){
	// Encender RS para mandar un dato
	PORTC |= (1<<PORTC0);
	
	// Escribir el dato
	LCD_Port8(c);
	
	// Hacer la transisci?n de EN
	PORTC |= (1<<PORTC2);
	_delay_us(100);
	PORTC &= ~(1<<PORTC2);
}

void LCD_Set_Cursor8(char c, char f){
	char temp;
	if (f == 1){
		temp = 0x80 + c - 1;
		LCD_CMD8(temp);
	}else if (f == 2){
		temp = 0xC0 + c - 1;
		LCD_CMD8(temp);
	}
}

void LCD_Write_String8(char *a){
	int i;
	for (i = 0; a[i] != '\0'; i++){
		LCD_Write_Char8(a[i]);
	}
}