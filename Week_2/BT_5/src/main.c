#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_usart.h"
#include "misc.h"
#include "uart.h"


int main(){
    while(1){

    }
}

void pwn_cfg(void){
    GPIO_InitTypeDef GPIO;
    TIM_TimeBaseInitTypeDef TIM;
    TIM_OCInitTypeDef PWM;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);

    GPIO.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO.GPIO_Pin = GPIO_Pin_0;
    GPIO_Init(GPIOA, &GPIO);

    TIM.TIM_Prescaler = 719;
    TIM.TIM_Period = 999;
    TIM.TIM_CounterMode = TIM_CounterMode_Up;
    TIM.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM2, &TIM);

    PWM.TIM_OCMode = TIM_OCMode_PWM1;
    PWM.TIM_OutputState = TIM_OutputState_Enable;
    PWM.TIM_OCPolarity = TIM_OCPolarity_High;
    PWM.TIM_Pulse = 0;

    TIM_OC1Init(TIM2, &PWM);
    TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);

    TIM_ARRPreloadConfig(TIM2, ENABLE);

    TIM_Cmd(TIM2, ENABLE);
}

void change_duty(uint8_t duty){
    TIM_SetCompare1(TIM2, duty *10);
}