#include "stm32f10x.h"
#include "button.h"
#include "led.h"

int main(void)
{
    uint8_t input_data;
    uint8_t output_data;

    button_cfg();
    led_cfg();

    while (1)
    {
        input_data = button_read();

        output_data = (uint8_t)(~input_data);
        led_write(output_data);
    }
}
