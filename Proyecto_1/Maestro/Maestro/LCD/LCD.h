/*
 * LCD.h
 *
 * Created: 1/27/2026 10:24:04 AM
 *  Author: admin
 */ 


#ifndef LCD_H_
#define LCD_H_
#define F_CPU 16000000
#include <util/delay.h>
#include <avr/io.h>

// LCD usando 8 bits
// Funci?n para inicializar LCD en modo de 8 bits
void init_LCD8(void);

void LCD_Port8(char a);

void LCD_CMD8(char a);

void LCD_Write_Char8(char c);

void LCD_Write_String8(char *a);

void LCD_Shift_Right8(void);

void LCD_Set_Cursor8(char c, char f);







#endif /* LCD_H_ */