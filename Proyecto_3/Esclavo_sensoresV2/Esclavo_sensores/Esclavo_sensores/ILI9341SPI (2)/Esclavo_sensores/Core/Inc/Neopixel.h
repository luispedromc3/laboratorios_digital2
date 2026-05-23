/*
 * Neopixel.h
 *
 *  Created on: May 13, 2026
 *      Author: admin
 */

#ifndef INC_NEOPIXEL_H_
#define INC_NEOPIXEL_H_

#include "main.h"
#include "math.h"

// Numero de LEDS
#define numPixels 4

// Valores de ancho de pulso del uno y el cero
#define CCR_0 34
#define CCR_1 67

// Colocar el timer y el canal usado
extern TIM_HandleTypeDef htim1;


// Colocar el timer usado
#define neoPixel_timer htim1
#define neoPixel_canal TIM_CHANNEL_1


// Funcion para aplicar el brillo con la funcion gamma
#define GAMMA_CORRECTION 2.2f // Valor de correcion de gamma adaptada al ojo humano
#define GAMMA 2.2f // Valor de correcion de gamma
#define MAX_BRIGHTNESS 255 // Valor maximo del brillo

void setPixelColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b);
void setBrightness(uint8_t b);
void pixelShow(void);
void pixelClear(void);

uint8_t Gamma_correction(uint8_t color, float brillo_);


#endif /* INC_NEOPIXEL_H_ */
