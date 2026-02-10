/*
 * SPI.h
 *
 * Created: 2/3/2026 9:41:22 AM
 *  Author: admin
 */ 


#ifndef SPI_H_
#define SPI_H_

#include <avr/io.h>

// Variables
typedef enum{
	SPI_MASTER_OSC_DIV2		= 0b01010000,
	SPI_MASTER_OSC_DIV4		= 0b01010001,
	SPI_MASTER_OSC_DIV8		= 0b01010010,
	SPI_MASTER_OSC_DIV16	= 0b01010011,
	SPI_MASTER_OSC_DIV32	= 0b01010100,
	SPI_MASTER_OSC_DIV64	= 0b01010101,
	SPI_MASTER_OSC_DIV128	= 0b01010110,
	SPI_SLAVE_SS			= 0b01000000
}SPI_TYPE;

typedef enum{
	SPI_DATA_ORDER_MSB		= 0b00000000,
	SPI_DATA_ORDER_LSB		= 0b00100000
}SPI_DATA_ORDER;

typedef enum{
	SPI_MODE0				= 0b00000000,
	SPI_MODE1				= 0b00000100,
	SPI_MODE2				= 0b00001000,
	SPI_MODE3				= 0b00001100
}SPI_CLOCK_MODE;


// Funciones de la libreria
void init_spi(SPI_TYPE, SPI_DATA_ORDER, SPI_CLOCK_MODE);
void spi_write(uint8_t dat);
uint8_t spi_read();
unsigned spiDataready();
 //void spiReceiveWait();
#endif /* SPI_H_ */