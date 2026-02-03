/*
 * Laboratorio_2.c
 *
 * Created: 1/27/2026 10:05:12 AM
 * Author : luisp
 */ 
#define F_CPU 16000000
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "LCD_8B/LCD_8.h"
#include "ADC/ADC.h"
uint8_t contador;
char adc6;
char adc7;
char adc_str1[6];
char adc_str2[6];
int main(void)
{
    /* Replace with your application code */
	cli();
	initLC8bits();
	initADC();
	/*
	LCD_SetCursor(1,1);
	LCD_WriteChar('F');
	LCD_WriteChar('u');
	LCD_WriteChar('n');
	LCD_WriteChar('c');
	LCD_WriteChar('i');
	LCD_WriteChar('o');
	LCD_WriteChar('n');
	LCD_SetCursor(1,2);
	LCD_WriteChar('G');
	*/
	LCD_SetCursor(1,1);
	LCD_WriteString("S1: ");
	LCD_SetCursor(1,2);
	LCD_WriteString("S2: ");
	contador = 0;
	adc6 = 0;
	adc7 = 0;
	sei();
    while (1) 
    {
    }
}
ISR(ADC_vect){
	//En esta interrupcion, leo y despues cambio al siguiente adc a leer.
	// verifico si estamos en modo 1 (modo manual), si no estamos en este modo no modifico el PWM.
	switch(contador){
		case 1:
			//char temp1 = ((ADCH*30/255) +7);
			//adc6 = (ADCH / 26) + '0';
			adc6 = ADCH* 999 / 1023 ;
			LCD_SetCursor(5,1);
			adc_str1[0] = (adc6 % 5) + '0';      // entero
			adc_str1[1] = '.';
			adc_str1[2] = (adc6 % 10) + '0';
			adc_str1[3] = (adc6 % 10) + '0';
			adc_str1[4] = 'V';
			adc_str1[5] = '\0';
			LCD_WriteString(adc_str1);
			ADMUX = 0;
			ADMUX |= (1 << REFS0) ; //VCC COMO COMPARADOR
			ADMUX |= (1 << ADLAR) ; //JUSTIFICADO A LA IZQUIERDA USAR HIGH
			ADMUX |= (1 << MUX2) | (1 << MUX1) | (1 << MUX0); // ADC7
			break;
		case 2:
			adc7 = ADCH* 999 / 1023 ;
			LCD_SetCursor(5,2);
			adc_str2[0] = (adc7 % 5) + '0';      // entero
			adc_str2[1] = '.';
			adc_str2[2] = (adc7 % 10) + '0';
			adc_str2[3] = (adc7 % 10) + '0';
			adc_str2[4] = 'V';
			adc_str2[5] = '\0';
			LCD_WriteString(adc_str2);
			ADMUX = 0;
			ADMUX |= (1 << REFS0) ; //VCC COMO COMPARADOR
			ADMUX |= (1 << ADLAR) ; //JUSTIFICADO A LA IZQUIERDA USAR HIGH
		
			ADMUX |= (1 << MUX2)|(1 << MUX1); //ADC6
			contador = 0;
			break;
		default:
			break;
	}
	contador++;
	
	ADCSRA |= (1 << ADSC); //EMPEZAR EL ADC
}
