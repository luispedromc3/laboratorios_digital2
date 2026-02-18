/*
 * I2C.h
 *
 * Created: 2/9/2026 8:03:04 PM
 *  Author: admin
 */ 


#ifndef I2C_H_
#define I2C_H_


#ifndef F_CPU
#define F_CPU 16000000
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>



// Funcion para inicializar el maestro
void I2C_Master_Init(unsigned long SCL_Clock, uint8_t Prescaler);

// Funcion de inicio de la comunicacion I2C
uint8_t I2C_Master_Start(void);
uint8_t I2C_Master_RepeatedStart(void);

// Funcion de parar de comunicacion I2C
void I2C_Master_stop(void);


// Funcion de transmision de datos del maestro al eslclavo
// esta funcion devolvera un cero si el esclavo ha recibido el dato
uint8_t I2C_Master_Write(uint8_t dato);

// Funcion de recepcion de datos enviados por el esclavo al maestro, funciona para
// leer los datos que estan en el escalvo
uint8_t I2C_Master_Read(uint8_t *buffer, uint8_t ack);


// Incializar la I2C del esclavo
void I2C_Slave_Init(uint8_t adress);


#endif /* I2C_H_ */