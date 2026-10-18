#ifndef PWM_H
#define PWM_H

#include "stm32f10x_conf.h"

#define PWM_PERIOD 1000

void PWM_Init(void);
void PWM_SetDutyCycle(uint8_t channel, uint16_t duty);

#endif