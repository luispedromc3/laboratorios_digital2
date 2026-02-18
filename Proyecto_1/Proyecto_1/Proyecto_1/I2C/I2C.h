/*
 * I2C.h
 *
 * Created: 2/10/2026 12:11:34 PM
 *  Author: luisp
 */ 


#ifndef I2C_H_
#define I2C_H_

#ifndef F_CPU
#define F_CPU 16000000
#endif
#include <avr/io.h>
#include <stdint.h>	
//I2C Maestro
void I2C_MasterInit(unsigned long SCL_Clock, uint8_t Prescaler);
//Comenzar Comunicacion I2C
uint8_t I2C_MasterStart(void);
uint8_t I2C_MasterRepeatedStart(void);
//Terminar Comunicacion I2C
uint8_t I2C_MasterStop(void);
//Transmision de datos del maestro al esclavo
//0 = dato recibido
uint8_t I2C_MasterWrite(uint8_t dato);
//Recepcion de datos del esclavo. Leer el dato del escalvo
uint8_t I2C_MasterRead(uint8_t *buffer, uint8_t ack);

//Esclavo
void I2C_SlaveInit(uint8_t address);




#endif /* I2C_H_ */