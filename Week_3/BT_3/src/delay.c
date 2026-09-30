#include "delay.h"

volatile uint32_t ms_ticks = 0;

void Timer2_cfg(void)
{
    TIM_TimeBaseInitTypeDef Delay;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    Delay.TIM_Prescaler = 71;
    Delay.TIM_Period = 999;
    Delay.TIM_ClockDivision = TIM_CKD_DIV1;
    Delay.TIM_CounterMode = TIM_CounterMode_Up;
    Delay.TIM_RepetitionCounter = 0;

    TIM_TimeBaseInit(TIM2, &Delay);

    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

    NVIC_Init(&NVIC_InitStructure);

    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);

    TIM_Cmd(TIM2, ENABLE);
}

void TIM2_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);

        ms_ticks++;
    }
}