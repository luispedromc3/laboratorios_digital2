#include "hx711.h"
#include <util/delay.h>
#include <avr/interrupt.h>

// ===== Variables globales =====
long hx711_offset = 0;
float hx711_scale = 104;



// ===== Inicializar HX711 =====
void HX711_Init(void) {
	// DOUT como entrada
	HX711_DOUT_DDR &= ~(1 << HX711_DOUT_PIN);
	HX711_DOUT_PORT |= (1 << HX711_DOUT_PIN); // pull-up opcional

	// CLK como salida
	HX711_CLK_DDR |= (1 << HX711_CLK_PIN);
	HX711_CLK_PORT &= ~(1 << HX711_CLK_PIN);
}

// ===== Leer datos RAW =====
long HX711_ReadRaw(void) {
	unsigned long count = 0;
	uint8_t i;

	// Esperar a que HX711 est? listo
	while (HX711_DOUT_PINR & (1 << HX711_DOUT_PIN));

	// Desactivar interrupciones SOLO en lectura cr?tica
	cli();

	for (i = 0; i < 24; i++) {
		HX711_CLK_PORT |= (1 << HX711_CLK_PIN);
		_delay_us(1);

		count <<= 1;
		if (HX711_DOUT_PINR & (1 << HX711_DOUT_PIN)) count++;

		HX711_CLK_PORT &=~(1 << HX711_CLK_PIN);
		_delay_us(1);
	}

	// Pulso extra para ganancia 128
	HX711_CLK_PORT |= (1 << HX711_CLK_PIN);
	_delay_us(2);
	HX711_CLK_PORT &= ~(1 << HX711_CLK_PIN);
	_delay_us(2);

	sei(); // activar interrupciones

	// Extensi?n de signo 24 bits
	if (count & 0x800000) {
		count |= 0xFF000000;
	}

	return (long)count;
}

// ===== TARE =====
void HX711_Tare(void) {
	long sum = 0;
	for(uint8_t i = 0; i < 30; i++){
		sum += HX711_ReadRaw();
	}
	hx711_offset = sum / 30;
}

// ===== Calibraci?n =====
void HX711_SetScale(float known_weight) {
	long raw = HX711_ReadRaw() - hx711_offset;
	hx711_scale = (float)raw / known_weight;
}

// ===== Obtener peso =====
float HX711_GetWeight(void) {
	long raw = HX711_ReadRaw() - hx711_offset;
	return (float)raw / hx711_scale;
}

// Leer promedio para estabilidad
long HX711_ReadAverage(uint8_t samples) {
	long sum = 0;
	for (uint8_t i = 0; i < samples; i++) {
		sum += HX711_GetWeight();
	}
	return sum / samples;
}