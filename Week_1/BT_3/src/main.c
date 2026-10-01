/* Bài tập 03 - đọc input, đảo bit và điều khiển dãy LED. */

#include "gpio.h"
#include "led.h"


int main(void)
{
    GPIO_Init();
    LED_Init();

    while (1)
    {
        uint8_t input = GPIO_ReadInput();
        uint8_t output_levels = (uint8_t)(~input);

        /* 0 thành 1, 1 thành 0 rồi ghi mức vật lý ra LED. */
        LED_Write(output_levels);
    }
}
