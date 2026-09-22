#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_usart.h"
#include "misc.h"
#include "uart.h"
#include "pwm.h"

volatile uint8_t ledstate = 0;  // trang thai led
volatile uint8_t pwmstate = 0; // trang thai pwm

void pwm_cfg(void);
void change_duty(uint8_t duty);

int main(){
    pwm_cfg();
    uart_cfg();
    while(1){

    }
}

