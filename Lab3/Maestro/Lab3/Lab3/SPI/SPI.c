/*
 * SPI.c
 *
 * Created: 2/3/2026 9:41:37 AM
 *  Author: admin
 */ 

#include "SPI.h"

void init_spi(SPI_TYPE sType,SPI_DATA_ORDER sDataorder, SPI_CLOCK_MODE sClockmode){
	 //PB2 ---> SS
	 //PB3 ---> MOSI
	 //PB4 ---> MISO
	 //PB5 ---> SCK
	 
	 if (sType & (1 << MSTR)){ // Verificar si se quiere configurar como maestro
		 DDRB |= (1 << DDB3) | (1 << DDB5) | (1 << DDB2); // Mosi, SCK, SS negado
		 DDRB &= ~(1 << DDB4); // MISO
		 PORTB |= (1 << PORTB2); // SS en HIGH
		 SPCR |= (1 << MSTR); // FUERZA MASTER

		 
		 uint8_t temp = sType & 0b00000111;
		 switch(temp){
			 case 0: // DIV2
				SPCR &= ~((1 << SPR1) | (1 << SPR0));
				SPSR |= (1 << SPI2X);
				break;
			case 1: //DIV 4
				SPCR &= ~((1<<SPR1)|(1<<SPR0));
				SPSR &= ~(1<<SPI2X);
				break;
			case 2: //DIV8
				SPCR |= (1<<SPR0);
				SPCR &= ~(1<<SPR1);
				SPSR |= (1<<SPI2X);
				break;
			case 3: //DIV16
				SPCR |= (1<<SPR0);
				SPCR &= ~(1<<SPR1);
				SPSR &= ~(1<<SPI2X);
				break;
			case 4: //DIV32
				SPCR &= ~(1<<SPR0);
				SPCR |= (1<<SPR1);
				SPSR |= (1<<SPI2X);
				break;
			case 5: //DIV64
				SPCR &= ~(1<<SPR0);
				SPCR |= (1<<SPR1);
				SPSR &= ~(1<<SPI2X);
				break;
			case 6: //DIV128
				SPCR |= (1<<SPR0)|(1<<SPR1);
				SPSR &= ~(1<<SPI2X);
				break;	
		 } 
	 }else{ // Significa que se configurara como esclavo
		 DDRB |= (1 << DDB4); // MISO
		 DDRB &= ~((1 << DDB3) | (1 << DDB5) | (1 << DDB2)); // MOSI, SCK, SS negado
		 SPCR &= ~(1 << MSTR);
	 }
	 
	 // Registro de control 
	 SPCR |= (1 << SPE) | sDataorder | sClockmode; 
}



void spi_write(uint8_t dat){
	SPDR = dat;
}

unsigned spiDataready(){
	if (SPSR & (1 << SPIF))
	return 1;
	else 
	return 0;
}

uint8_t spi_read(){
	while (!(SPSR & (1 << SPIF)));
	return(SPDR);
}

