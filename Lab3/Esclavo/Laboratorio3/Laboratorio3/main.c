/*
 * NombreProgra.c
 *
 * Created: 2/3/2026 10:08:36 AM
 * Author: Luis Pedro 
 * Description: Laboratorio 3 electronica digital 2
 */
/****************************************/
// Encabezado (Libraries)
#include <avr/io.h>
#include "UART/UART.h"
#include <stdlib.h> // para itoa()
#include <avr/interrupt.h>
#include "SPI/SPI.h"
#include "ADC/ADC.h"
uint8_t contador;
uint8_t contador_adc;
volatile uint8_t val_adc6;
uint8_t val_adc7;

/****************************************/
// Function prototypes
void setup();
void mostrarContador(uint8_t contador);
/****************************************/
// Main Function
int main(void)
{
	/* Replace with your application code */
	cli();
	setup();
	initUART();
	initADC();
	SpiInit(SPI_SLAVE_SS, SPI_DATA_ORDER_MSB, SPI_MODE0);
	SPCR0 |= (1 << SPIE);
	sei();
	
	while (1)
	{
		WriteString("ADC 6: ");
		writeNumber(val_adc6);
		WriteString("\n");
		WriteString("ADC 7: ");
		writeNumber(val_adc7);
		WriteString("\n");
	}
}
/****************************************/
// NON-Interrupt subroutines
void setup(){
	//Contador como salida
	DDRD |= (1 << DDD2) | (1 << DDD3) | (1 << DDD4) |(1 << DDD5) | (1 << DDD6) | (1 << DDD7);
	DDRB |= (1 << PORTB0) | (1 << PORTB1); 
	// Contador apagado
	PORTD &= ~((1 << PORTD2) | (1 << PORTD3) | (1 << PORTD4) |(1 << PORTD5) | (1 << PORTD6) | (1 << PORTD7));
	PORTB &= ~(((1 << PORTB0)) | (1 << PORTB1));
	contador = 0;
	contador_adc = 0;
	val_adc6 = 0;
	val_adc7 = 0;
}
void mostrarContador(uint8_t contador)
{
	// PD2–PD7
	PORTD = (PORTD & 0x03) | (contador << 2);

	// PB0–PB1
	PORTB = (PORTB & 0xFC) | (contador >> 6);
}
/****************************************/
// Interrupt routines
ISR(USART0_RX_vect){
	// por ahora devolvere lo que recibo
	char temporal = UDR0;
	if (temporal == '1'){
		contador = 255;
		mostrarContador(contador);
	}else if (temporal == '0'){
		contador = 0;
		mostrarContador(contador);
	}else if (temporal == '6'){
		contador = val_adc6;
		mostrarContador(contador);
		
	}else if (temporal == '7'){
		contador = val_adc7;
		mostrarContador(contador);
	}else{
		WriteCharUART(temporal);
		contador = 0;
		mostrarContador(contador);
	}
	
}

ISR(ADC_vect){
	//En esta interrupcion, leo y despues cambio al siguiente adc a leer.
	// verifico si estamos en modo 1 (modo manual), si no estamos en este modo no modifico el PWM.
	switch(contador_adc){
		case 0:
			//char temp1 = ((ADCH*30/255) +7);
			val_adc6 = ADCH;
			ADMUX = 0;
			ADMUX |= (1 << REFS0) ; //VCC COMO COMPARADOR
			ADMUX |= (1 << ADLAR) ; //JUSTIFICADO A LA IZQUIERDA USAR HIGH
			ADMUX |= (1 << MUX2) | (1 << MUX1) | (1 << MUX0); // ADC7
			contador_adc++;
		break;
		case 1:
			//adc7 = ADCH* 999 / 1023 ;
			val_adc7 = ADCH;
			ADMUX = 0;
			ADMUX |= (1 << REFS0) ; //VCC COMO COMPARADOR
			ADMUX |= (1 << ADLAR) ; //JUSTIFICADO A LA IZQUIERDA USAR HIGH
		
			ADMUX |= (1 << MUX2)|(1 << MUX1); //ADC6
			contador_adc = 0;
		break;
		default:
		break;
	
	
	}
	ADCSRA |= (1 << ADSC); //EMPEZAR EL ADC
}
ISR(SPI0_STC_vect){
	uint8_t dato = SPDR0;
	uint8_t dato_led = 0;
	//PORTD ^= (1<< PORTD2);
	if (dato == 'a'){
		SPDR0 = val_adc6;
	}else if (dato == 'b'){
		SPDR0 = val_adc7;
	}else{
		dato_led = atoi(dato)
		mostrarContador(dato_led);
		//PORTD ^= (1<< PORTD2);
		//SPDR0= 111;
	}
	//PORTB ^= (1<< PORTB1);
	
}


//uint8_t spiValor = SPDR0;
	//SPDR0 = 0xFF;
	/*if(spiValor == 'c'){
		SPDR0 = 0xFF;
	}
	*/