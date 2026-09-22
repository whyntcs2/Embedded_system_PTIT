#ifndef __systick__H
#define __systick__H

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "misc.h"

void systick_cfg(void);
void SysTick_Handler(void);

#endif