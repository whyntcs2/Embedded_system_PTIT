#ifndef __DELAY__H
#define __DELAY__H

#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_spi.h"
#include "stm32f10x_tim.h"
#include "misc.h"
#include "uart.h"

extern volatile uint32_t ms_ticks;

void Timer2_cfg(void);

#endif