/*
 * PWM2.h
 *
 * Created: 4/30/2025 8:57:55 AM
 *  Author: admin
 */ 


#ifndef PWM0_H_
#define PWM0_H_

#include <avr/io.h>

#define invert 1
#define no_invert 0


void initPWMFastA0(uint8_t invertido, uint16_t prescaler);
void initPWMFastB0(uint8_t invertido, uint16_t prescaler);

void updateDutyCycleA0(uint8_t dutycycle);
void updateDutyCycleB0(uint8_t dutycycle);

#endif /* PWM0_H_ */