/*
 * SPI.c
 *
 * Created: 2/3/2026 11:29:36 AM
 *  Author: luisp
 */ 
#define F_CPU 16000000
#include <avr/io.h>
#include <stdint.h> // para itoa()
#include <avr/interrupt.h>
#include <util/delay.h>
#include "SPI.h"


void SpiInit(Spi_type type, Spi_Data_Order sData, SPI_CLOCK_MODE SMode){
	if(type & (1<<MSTR)){
		DDRB |= (1<<DDB3) | (1<<DDB5) | (1<<DDB2);
		DDRB &= ~(1<<DDB4);
		SPCR0 |= (1<<MSTR);
		uint8_t temp = type & 0b00000111;
		switch(temp){
			case 0 : //div 2
				SPCR0 &= ~((1<< SPR1)|(1<<SPR0));
				SPSR0 |= (1<< SPI2X);
			break;
			case 1: //div 4
				SPCR0 &= ~((1<< SPR1)|(1<<SPR0));
				SPSR0 &= ~(1<< SPI2X);
			break;
			case 2: //div 8
				SPCR0 |= (1<<SPR0);
				SPCR0 &= ~(1<< SPR1);
				SPSR0 |= (1<< SPI2X);
			break;
			case 3: //div 16
				SPCR0 |= (1<<SPR0);
				SPCR0 &= ~(1<< SPR1);
				SPSR0 &= ~(1<< SPI2X);
			break;
			case 4: // div 32
				SPCR0 &= ~(1<<SPR0);
				SPCR0 |= (1<< SPR1);
				SPSR0 |= (1<< SPI2X);
			break;
			case 5: //64
				SPCR0 &= ~(1<<SPR0);
				SPCR0 |= (1<< SPR1);
				SPSR0 &= ~(1<< SPI2X);
			break;
			case 6:
				SPCR0 |= (1<<SPR0);
				SPCR0 |= (1<< SPR1);
				SPSR0 &= ~(1<< SPI2X);
			break;
			default:
			break;
		}
	}else{
		DDRB |= (1<<DDB4);
		DDRB &= ~((1<<DDB3) | (1<<DDB5) | (1<<DDB2));
		SPCR0 &= ~(1 << MSTR);
	}
	SPCR0 |= (1<<SPE)|sData|SMode;
	
}
/*
static void spiReceiveWait(){
	while (!(SPSR0 & (1<< SPIF)));
	
}
*/
void Spi_Write(uint8_t dat){
	SPDR0 = dat;
}
unsigned Spi_DataReady(){
	if(SPSR0 & (1<< SPIF)){
		return 1;
	}else{
		return 0;
	}
}
uint8_t SpiRead(void){
	while(!(SPSR0 & (1<<SPIF))){
		
	}
	return(SPDR0);
	
}
