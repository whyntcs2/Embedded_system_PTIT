/* Driver cho dãy LED của bài tập 03. */

#include "gpio.h"
#include "led.h"

#define GPIOA_LED_MASK  ((0x1FUL << 8) | (1UL << 15))
#define GPIOB_LED_MASK  ((1UL << 12) | (1UL << 13))


void LED_Init(void)
{
    uint32_t pin;

    /* Chốt mức HIGH trước khi chuyển chân sang output để LED tắt lúc khởi động. */
    GPIO_WriteMasked(GPIOA, GPIOA_LED_MASK, GPIOA_LED_MASK);
    GPIO_WriteMasked(GPIOB, GPIOB_LED_MASK, GPIOB_LED_MASK);

    /* PA13/PA14 không đụng tới vì đang dành cho SWD. */
    for (pin = 8UL; pin <= 12UL; pin++)
    {
        GPIO_ConfigOutputPin(GPIOA, pin);
    }
    GPIO_ConfigOutputPin(GPIOA, 15UL);

    /* PB13 thay PA13, PB12 thay PA14 trong thứ tự dữ liệu LED. */
    GPIO_ConfigOutputPin(GPIOB, 12UL);
    GPIO_ConfigOutputPin(GPIOB, 13UL);
}


void LED_Write(uint8_t output_levels)
{
    uint32_t gpioa_value;
    uint32_t gpiob_value = 0UL;

    /* bit 0..4 -> PA8..PA12; bit 7 -> PA15. */
    gpioa_value = ((uint32_t)(output_levels & 0x1FU) << 8) |
                  ((uint32_t)(output_levels & 0x80U) << 8);

    /* bit 5 -> PB13; bit 6 -> PB12. */
    if ((output_levels & 0x20U) != 0U)
    {
        gpiob_value |= (1UL << 13);
    }
    if ((output_levels & 0x40U) != 0U)
    {
        gpiob_value |= (1UL << 12);
    }

    GPIO_WriteMasked(GPIOA, GPIOA_LED_MASK, gpioa_value);
    GPIO_WriteMasked(GPIOB, GPIOB_LED_MASK, gpiob_value);
}
