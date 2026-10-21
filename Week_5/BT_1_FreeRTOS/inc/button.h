#ifndef BUTTON_H
#define BUTTON_H

#include "stm32f10x_conf.h"

void Button_Init(void);
uint8_t Button_ReadPin(uint16_t pin);

#endif