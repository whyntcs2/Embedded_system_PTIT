#ifndef __uart__H
#define __uart__H

#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include "misc.h"

#define RX_Size 256
#define TX_Size 300

extern volatile char rx_buffer[RX_Size];
extern volatile char tx_buffer[TX_Size];

extern volatile uint16_t rx_index;
extern volatile uint16_t tx_index;
extern volatile uint16_t tx_length;

void uart_cfg(void);
uint8_t uart_send_string_it(const char *str);


#endif