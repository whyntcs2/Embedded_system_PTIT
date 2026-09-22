#ifndef __USART_H
#define __USART_H

#include "stm32f10x.h"

void USART1_Init(uint32_t baudrate);
void USART1_SendString(char *str);

#endif
