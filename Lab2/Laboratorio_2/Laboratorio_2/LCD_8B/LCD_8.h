/*
 * LCD_8.h
 *
 * Created: 1/27/2026 10:06:57 AM
 *  Author: luisp
 */ 


#ifndef LCD_8_H_
#define LCD_8_H_
#define F_CPU 16000000
#include <avr/io.h>
#include <util/delay.h>

void initLC8bits(void);
void LCD_port(char a);
void LCD_CMD(char a);
void LCD_WriteChar(char a);
void LCD_WriteString(char *a);
void LCD_ShiftRight(void);
void LCD_ShiftLeft(void);
void LCD_SetCursor(char c, char f);
#endif /* LCD_8_H_ */