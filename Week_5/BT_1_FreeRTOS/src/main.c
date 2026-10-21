#include "stm32f10x_conf.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "queue.h"
#include "pwm.h"
#include "button.h"
#include "app_tasks.h"

void vApplicationMallocFailedHook(void) {
    taskDISABLE_INTERRUPTS();
    while (1);
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) {
    (void)xTask;
    (void)pcTaskName;
    taskDISABLE_INTERRUPTS();
    while (1);
}

int main(void) {
    SystemInit();
    PWM_Init();
    Button_Init();

    for (int i = 0; i < 3; i++) {
        xLedQueue[i] = xQueueCreate(5, sizeof(uint16_t));
    }

    xTaskCreate(vLEDTask, "LED_Task_0", configMINIMAL_STACK_SIZE, (void *)(uintptr_t)0, tskIDLE_PRIORITY + 1, NULL);
    xTaskCreate(vLEDTask, "LED_Task_1", configMINIMAL_STACK_SIZE, (void *)(uintptr_t)1, tskIDLE_PRIORITY + 1, NULL);
    xTaskCreate(vLEDTask, "LED_Task_2", configMINIMAL_STACK_SIZE, (void *)(uintptr_t)2, tskIDLE_PRIORITY + 1, NULL);

    xTaskCreate(vButtonTask, "Button_Task", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 2, NULL);

    vTaskStartScheduler();

    while (1) {
    }
}