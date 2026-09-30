#ifndef __DELAY_H
#define __DELAY_H

#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_tim.h"
#include "misc.h"

extern volatile uint32_t ms_ticks;

void Timer2_cfg(void);

#endif