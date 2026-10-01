#include "led.h"

void led_cfg(void)
{

    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    GPIOA->CRH = 0x33333333U;
    GPIOA->ODR &= 0x00FFU;
}

void led_write(uint8_t data)
{
    uint32_t odr;
    odr = GPIOA->ODR & 0x00FFU;
    odr |= ((uint32_t)data << 8);
    GPIOA->ODR = odr;
}
