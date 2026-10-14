#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "misc.h"

#include "FreeRTOS.h"
#include "task.h"

typedef struct
{
    GPIO_TypeDef *GPIOx;
    uint16_t GPIO_Pin;
    uint32_t frequency_mHz;
} LED_Config_t;

void LED_Init(void);
void LED_Blink(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, uint32_t frequency_mHz);
void LED_Task(void *pvParameters);
void vApplicationMallocFailedHook(void);
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName);

static LED_Config_t LED_Config[3] =
{
    {GPIOC, GPIO_Pin_13, 100},
    {GPIOA, GPIO_Pin_0, 1000},
    {GPIOA, GPIO_Pin_1, 10000}
};

int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    LED_Init();
    if (xTaskCreate(LED_Task, "LED_01", 128, &LED_Config[0], 1, NULL) != pdPASS)
    {
        while (1);
    }

    if (xTaskCreate(LED_Task, "LED_02", 128, &LED_Config[1], 1, NULL) != pdPASS)
    {
        while (1);
    }
    if (xTaskCreate(LED_Task, "LED_03", 128, &LED_Config[2], 1, NULL) != pdPASS)
    {
        while (1);
    }
    vTaskStartScheduler();
    while (1);
}

/* Cau hinh GPIO */
void LED_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC, ENABLE);
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin =GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_SetBits(GPIOC, GPIO_Pin_13);
    GPIO_ResetBits(GPIOA, GPIO_Pin_0 | GPIO_Pin_1);
}

void LED_Blink(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, uint32_t frequency_mHz)
{
    TickType_t xLastWakeTime;
    TickType_t xHalfPeriod;
    BitAction LED_State;
    uint32_t halfPeriodMs = 500000UL / frequency_mHz;
    xHalfPeriod = pdMS_TO_TICKS(halfPeriodMs);
    xLastWakeTime = xTaskGetTickCount();
    LED_State = (GPIOx == GPIOC) ? Bit_SET : Bit_RESET;
    while (1)
    {
        LED_State = (LED_State == Bit_SET)? Bit_RESET : Bit_SET;
        GPIO_WriteBit(GPIOx, GPIO_Pin, LED_State);
        vTaskDelayUntil(&xLastWakeTime, xHalfPeriod);
    }
}


void LED_Task(void *pvParameters)
{
    LED_Config_t *config = (LED_Config_t *)pvParameters;
    LED_Blink(config->GPIOx, config->GPIO_Pin, config->frequency_mHz);
}

void vApplicationMallocFailedHook(void)
{
    taskDISABLE_INTERRUPTS();
    while (1);
}

void vApplicationStackOverflowHook(
    TaskHandle_t xTask, char *pcTaskName)
{
    (void)xTask;
    (void)pcTaskName;

    taskDISABLE_INTERRUPTS();
    while (1);
}
