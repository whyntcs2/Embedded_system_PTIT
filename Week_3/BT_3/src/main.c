#include "stm32f10x.h"

#include "uart.h"
#include "button.h"
#include "delay.h"

int main(void)
{
    uint16_t cnt;
    uint16_t previous_cnt = 0;

    uart_cfg();
    uart_dma_cfg();

    button_cfg();

    Timer2_cfg();

    while (1)
    {
        button_debounce();

        cnt = button_get_count();

        if ((cnt != previous_cnt) &&
            (uart_is_busy() == 0))
        {
            uart_prepare_message(cnt);

            uart_dma_send();

            previous_cnt = cnt;
        }
    }
}