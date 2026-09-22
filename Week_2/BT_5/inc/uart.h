#ifndef __uart__H
#define __uart__H

#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include "misc.h"
#include "pwm.h"

#define RX_Size 256
#define TX_Size 300

extern volatile char rx_buffer[RX_Size];
extern volatile char tx_buffer[TX_Size];



void uart_cfg(void);
void command(void);
void prepare_status(void);



#endif