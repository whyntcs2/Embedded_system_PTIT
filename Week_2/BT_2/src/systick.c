#include "systick.h"

volatile uint32_t fre_01 = 0; //0.1Hz
volatile uint32_t fre_02 = 0; //1Hz
volatile uint32_t fre_03 = 0; //10Hz

void systick_cfg(void){
    SysTick->LOAD = 71999;
    SysTick->VAL  = 0;
    SysTick->CTRL = 7;
}

void SysTick_Handler(void)
{
    fre_01++;
    fre_02++;
    fre_03++;

    if(fre_01 >= 5000){
        GPIOA -> ODR ^= GPIO_Pin_0;
        fre_01 = 0;
    }

    if(fre_02 >= 500){
        GPIOA -> ODR ^= GPIO_Pin_4;
        fre_02 = 0;
    }

    if(fre_03 >= 50){
        GPIOA -> ODR ^= GPIO_Pin_7;
        fre_03 = 0;
    }
}