#ifndef HX711_H
#define HX711_H

#define F_CPU 16000000
#include <avr/io.h>
#include <stdint.h>

// ================= PINES (CAMBIA SI QUIERES) =================
// DOUT = PD2
// CLK  = PD3

#define HX711_DOUT_PIN   PORTC0
#define HX711_CLK_PIN    PORTC1

#define HX711_DOUT_PORT  PORTC
#define HX711_DOUT_DDR   DDRC
#define HX711_DOUT_PINR  PINC

#define HX711_CLK_PORT   PORTC
#define HX711_CLK_DDR    DDRC

// ================= FUNCIONES =================
void HX711_Init(void);
long HX711_ReadRaw(void);

void HX711_Tare(void);
void HX711_SetScale(float known_weight);
float HX711_GetWeight(void);

long HX711_ReadAverage(uint8_t samples);

// ================= VARIABLES =================
extern long hx711_offset;
extern float hx711_scale;

#endif