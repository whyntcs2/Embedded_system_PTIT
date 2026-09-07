#include "button.h"

void button_cfg(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    GPIOA->CRL = 0x88888888U;
    GPIOA->ODR |= 0x00FFU;
}

uint8_t button_read(void)
{
    return (uint8_t)(GPIOA->IDR & 0x00FFU);
}
