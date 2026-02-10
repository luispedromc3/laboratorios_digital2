/*
 * Lab3.c
 *
 * Created: 2/3/2026 9:38:22 AM
 * Author : Matheo Echeverria
 * Laboratorio del´la comunicación SPI
 */ 

// Libraries
#define F_CPU 1000000
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdlib.h>
#include <util/delay.h>
#include "UART/UART.h"
#include "ADC/ADC.h"
#include "SPI/SPI.h"

// Prototype functions
void setup();
void leds(uint8_t a);

// Variables
uint8_t valorSPI = 0;
char buffer[10];
char valorLED[4];
uint8_t LED;
uint8_t indx = 0;
uint8_t led_ready;

// Main loop
int main(void)
{
	setup();
    while (1) 
    {
		PORTB &= ~(1 << PORTB2); // SS = 0

		spi_write('a');
		spi_write(0x00);
		valorSPI = spi_read();
		utoa(valorSPI, buffer, 10);
		writeString("Potenciometro 1: ");
		writeString(buffer);
		//writeString("\n");
		
		PORTB |= (1 << PORTB2);  // SS = 1
		
		PORTB &= ~(1 << PORTB2); // SS = 0
		spi_write('b');
		spi_write(0x00);
		valorSPI = spi_read();
		utoa(valorSPI, buffer, 10);
		writeString("  Potenciometro 2: ");
		writeString(buffer);
		writeString("\n");
		
		
		
		

		PORTB |= (1 << PORTB2);  // SS = 1
		
		
		if (led_ready) {
			PORTB &= ~(1 << PORTB2); // SS = 0
			
			led_ready = 0;
			LED = atoi((char*)valorLED);
			leds(LED);
			
			spi_write(LED);
			
			
		}
		
		
		//_delay_ms(250);

		
    }
}

// NON-Interrupts subroutines
void setup(){
	cli();
	CLKPR = (1 << CLKPCE);
	CLKPR = (1 << CLKPS2); // FRECUENCIA DEL CLOCK PRINCIPAL DE 1 MHz
	// Inicializar pines de salida para los LEDS
	// PORTD 2-7
	DDRD |= (1<<PORTD2) | (1<<PORTD3) | (1<<PORTD4) | (1<<PORTD5) | (1<<PORTD6) | (1<<PORTD7);
	PORTD &= ~((1<<PORTD2) | (1<<PORTD3) | (1<<PORTD4) | (1<<PORTD5) | (1<<PORTD6) | (1<<PORTD7));
	// PORTB 0-1
	DDRB |= (1<<PORTB0) | (1 << PORTB1);
	PORTB &= ~((1<<PORTB0) | (1 << PORTB1));
	
	init_spi(SPI_MASTER_OSC_DIV4, SPI_DATA_ORDER_MSB,SPI_MODE0);
	initUART();

	PORTB |= (1 << PORTB2); // Es esl slave select. necesito que este en 1 porque es negado
	SPCR |= (1 << SPE);
	
	sei();
}

void leds(uint8_t a){
	uint8_t puerto_d = (a & 0b00111111) << 2;
	uint8_t puerto_b = (a & 0b11000000) >> 6; // Máscara para conservar únicamente los últimos 2 bits
	
	PORTD = (PORTD & 0b00000011) | puerto_d;
	PORTB = (PORTB & 0b11111100) | puerto_b;
}

// Interrupts
ISR(USART_RX_vect){
	char c = UDR0;
	
	 if (c == '\n' || c == '\r' || c == '.') {
		 valorLED[indx] = '\0';  // cerrar string
		 indx = 0;
		 led_ready = 1; 
	} else{
			valorLED[indx] = c;
			indx++;
	}
}