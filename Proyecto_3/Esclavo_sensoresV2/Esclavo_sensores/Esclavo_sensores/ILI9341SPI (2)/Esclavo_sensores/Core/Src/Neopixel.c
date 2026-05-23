/*
 * Neopixel.c
 *
 *  Created on: May 13, 2026
 *      Author: admin
 */

#include "Neopixel.h"
#include "main.h"
#include <math.h>

// matrix para almacenar datos del # de led y color
uint8_t LED_Data[numPixels][4];

// matrix si se quiere guardar eel brillo de las leds
uint8_t LED_Mod[numPixels][4];


// Tamaño de buffer para almacenar PWM
uint16_t pwmData[(24 * numPixels) + 50];

uint8_t datasentflag = 0;

extern float brilloled;

// Carga un RGB_Color deseado al led deseado
void setPixelColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b){
	LED_Data[n][0] = n;
	LED_Data[n][1] = g;
	LED_Data[n][2] = r;
	LED_Data[n][3] = b;
}


void setBrightness(uint8_t b){
	if (b > 100){
		b = 100;
	}

	brilloled = b;
	for (int i = 0; i < numPixels; i++){
		LED_Mod[i][0] = LED_Data[i][0];
		for (uint8_t j = 1; j < 4; j++){
			// Aplicar la correcion de gama y brillo
			int8_t brill = Gamma_correction(LED_Data[i][j], brilloled);
			LED_Mod[i][j] = brill;
		}
	}
}


void pixelShow(void){
	uint32_t RGB_Color = 0;
	uint32_t indx = 0;

	for (int i = 0; i < numPixels; i++){

		LED_Mod[i][0] = LED_Data[i][0];
		for (uint8_t j = 1; j < 4; j++) {
			// Aplicar correcion gamma y brillo
			int8_t brill = Gamma_correction(LED_Data[i][j], brilloled);
			LED_Mod[i][j] = brill;
		}

		RGB_Color = ((LED_Mod[i][1]) << 16 | (LED_Mod[i][2] << 8) | (LED_Mod[i][3]));

		for (int i = 23; i >= 0; i--){
			if (RGB_Color & (1 << i)){
				pwmData[indx] = CCR_1; // Poner 1
			}else{
				pwmData[indx] = CCR_0; // Poner 0
			}
			indx++;
		}

	}

	for (int i = 0; i < 50; i++){
		pwmData[indx] = 0;
		indx++; // Para mandar el reseet llenamos todos con ceros
	}

	HAL_TIM_PWM_Start_DMA(&neoPixel_timer, neoPixel_canal, (uint32_t*) pwmData, indx);

	while (!datasentflag);

	datasentflag = 0;
}

void pixelClear(void){
	for (uint8_t i = 0; i < numPixels; i++){
		LED_Data[i][0] = i;
		LED_Data[i][1] = 0;
		LED_Data[i][2] = 0;
		LED_Data[i][3] = 0;

	}
}


uint8_t  Gamma_correction(uint8_t c, float b){
	// Limitar el valor de brillo en un rango de 0 a 100
	if (b > 100.0f)
		b = 100.0f;

	if (b < 0.0f)
		b = 0.0f;

	// Se normaliza el brillo del rango  de 0.0 a 1.0
	float brillo_factor = b / 100.0f;

	// Aplicamos el factor
	float adjustedColor = c*brillo_factor;

	// Formula de escalamiento de gamma
	float gammaCorrectedColor = pow((adjustedColor / 255.0f), GAMMA) * 255.0f;

	// Analiza que este dentro de los limites
	if (gammaCorrectedColor > 255.0f)
		gammaCorrectedColor = 255.0f;

	if (gammaCorrectedColor < 0.0f)
		gammaCorrectedColor = 0.0f;

	// Se regresa el valor
	return (uint8_t) gammaCorrectedColor;

}



void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim){
	if(htim->Instance == TIM1){
		HAL_TIM_PWM_Stop_DMA(&neoPixel_timer, neoPixel_canal);
		datasentflag = 1;
	}
}

