#ifndef __UART_H
#define __UART_H

#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_dma.h"
#include "stm32f10x_usart.h"
#include "misc.h"

void uart_cfg(void);
void uart_dma_cfg(void);

void uart_prepare_message(uint16_t value);
void uart_dma_send(void);

uint8_t uart_is_busy(void);

#endif