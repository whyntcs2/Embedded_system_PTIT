#include "led.h"

void led_cfg(void){
    RCC -> APB2ENR |= 0x00000004;
    GPIOA->CRL &= ~(0xF << 16);
    GPIOA -> CRL |= 0x00030000;
    GPIOA->ODR |= (1 << 4);
}

