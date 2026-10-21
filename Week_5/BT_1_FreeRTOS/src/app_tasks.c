#include "app_tasks.h"
#include "pwm.h"
#include "button.h"

QueueHandle_t xLedQueue[3];

void vLEDTask(void *pvParameters) {
    uint8_t led_id = (uint8_t)(uintptr_t)pvParameters;
    uint16_t duty_cycle = 0;

    while (1) {
        if (xQueueReceive(xLedQueue[led_id], &duty_cycle, portMAX_DELAY) == pdTRUE) {
            PWM_SetDutyCycle(led_id, duty_cycle);
        }
    }
}

void vButtonTask(void *pvParameters) {
    (void)pvParameters;

    uint8_t selected_led = 0;
    uint16_t duty[3] = {500, 500, 500};

    uint8_t btn_select_last = 1;
    uint8_t btn_inc_last = 1;
    uint8_t btn_dec_last = 1;

    for (uint8_t i = 0; i < 3; i++) {
        xQueueSend(xLedQueue[i], &duty[i], 0);
    }

    while (1) {
        uint8_t btn_select_now = Button_ReadPin(GPIO_Pin_12);
        uint8_t btn_inc_now = Button_ReadPin(GPIO_Pin_13);
        uint8_t btn_dec_now = Button_ReadPin(GPIO_Pin_14);

        if (btn_select_now == Bit_RESET && btn_select_last == Bit_SET) {
            selected_led = (selected_led + 1) % 3;
        }

        if (btn_inc_now == Bit_RESET && btn_inc_last == Bit_SET) {
            if (duty[selected_led] <= PWM_PERIOD - 100) {
                duty[selected_led] += 100;
            } else {
                duty[selected_led] = PWM_PERIOD;
            }
            xQueueSend(xLedQueue[selected_led], &duty[selected_led], 0);
        }

        if (btn_dec_now == Bit_RESET && btn_dec_last == Bit_SET) {
            if (duty[selected_led] >= 100) {
                duty[selected_led] -= 100;
            } else {
                duty[selected_led] = 0;
            }
            xQueueSend(xLedQueue[selected_led], &duty[selected_led], 0);
        }

        btn_select_last = btn_select_now;
        btn_inc_last = btn_inc_now;
        btn_dec_last = btn_dec_now;

        vTaskDelay(pdMS_TO_TICKS(20));
    }
}