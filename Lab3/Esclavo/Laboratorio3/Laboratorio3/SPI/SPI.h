/*
 * SPI.h
 *
 * Created: 2/3/2026 11:29:46 AM
 *  Author: luisp
 */ 


#ifndef SPI_H_
#define SPI_H_
typedef enum{
	SPI_MODE0				= 0b00000000,
	SPI_MODE1				= 0b00000100,
	SPI_MODE2				= 0b00001000,
	SPI_MODE3				= 0b00001100
}SPI_CLOCK_MODE;

typedef enum
{
	SPI_MASTER_OSC_DIV2		 = 0b01010000,
	SPI_MASTER_OSC_DIV4		 = 0b01010001,
	SPI_MASTER_OSC_DIV8		= 0b01010010,
	SPI_MASTER_OSC_DIV16 = 0b01010011,
	SPI_MASTER_OSC_DIV32 = 0b01010100,
	SPI_MASTER_OSC_DIV64 = 0b01010101,
	SPI_MASTER_OSC_DIV128 = 0b01010110,
	SPI_SLAVE_SS =		0b01000000
}Spi_type;
typedef enum
{
	SPI_DATA_ORDER_MSB = 0x00,
	SPI_DATA_ORDER_LSB = 0b00100000
	
}Spi_Data_Order;
typedef enum
{
	SPI_CLOCK_IDLE_HIGH = 0b00001000,
	SPI_CLOCK_IDLE_LOW = 0x00
	
}Spi_Clock_Polarity;

typedef enum{
		SPI_CLOCK_FIRST_EDGE = 0x00,
		SPI_CLOCK_LAST_EDGE = 0b00000100
}Spi_Clock_Phase;
void SpiInit(Spi_type, Spi_Data_Order, SPI_CLOCK_MODE);
void Spi_Write(uint8_t dat);
unsigned Spi_DataReady();
uint8_t SpiRead(void);





#endif /* SPI_H_ */