#ifndef PWM_H
#define PWM_H

#include "stm32f10x.h"

extern volatile uint8_t ledstate;
extern volatile uint8_t pwmstate;

void pwm_cfg(void);
void change_duty(uint8_t duty);

#endif