#include "LED.h"

void gpio_cfg(void){
    GPIO_InitTypeDef LED;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    LED.GPIO_Mode = GPIO_Mode_Out_PP;
    LED.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_4 | GPIO_Pin_7;
    LED.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &LED);
}
