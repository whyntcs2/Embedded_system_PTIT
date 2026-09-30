#include "button.h"
#include "delay.h"

static volatile uint16_t button_count = 0;
static volatile uint8_t button_pending = 0;
static volatile uint32_t button_time = 0;

void button_cfg(void)
{
    GPIO_InitTypeDef Button;
    EXTI_InitTypeDef exti;
    NVIC_InitTypeDef nvic;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB |
                           RCC_APB2Periph_AFIO,
                           ENABLE);

    Button.GPIO_Mode = GPIO_Mode_IPU;
    Button.GPIO_Pin = GPIO_Pin_12;
    GPIO_Init(GPIOB, &Button);

    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,
                        GPIO_PinSource12);

    exti.EXTI_Line = EXTI_Line12;
    exti.EXTI_Mode = EXTI_Mode_Interrupt;
    exti.EXTI_Trigger = EXTI_Trigger_Falling;
    exti.EXTI_LineCmd = ENABLE;
    EXTI_Init(&exti);

    EXTI_ClearITPendingBit(EXTI_Line12);

    nvic.NVIC_IRQChannel = EXTI15_10_IRQn;
    nvic.NVIC_IRQChannelPreemptionPriority = 0;
    nvic.NVIC_IRQChannelSubPriority = 0;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic);
}

void EXTI15_10_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line12) != RESET)
    {
        button_time = ms_ticks;
        button_pending = 1;

        EXTI->IMR &= ~EXTI_Line12;

        EXTI_ClearITPendingBit(EXTI_Line12);
    }
}

void button_debounce(void)
{
    if (button_pending)
    {
        if ((ms_ticks - button_time) >= 20)
        {
            if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12) == Bit_RESET)
            {
                button_count++;
            }

            button_pending = 0;

            EXTI_ClearITPendingBit(EXTI_Line12);

            EXTI->IMR |= EXTI_Line12;
        }
    }
}

uint16_t button_get_count(void)
{
    return button_count;
}