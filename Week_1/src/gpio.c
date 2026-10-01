/* Driver GPIO dùng chung cho bài tập 03. */

#include "gpio.h"

#define GPIO_INPUT_MASK       (0x00FFUL)  /* PA0..PA7 */
#define GPIO_INPUT_PULLDOWN   (0x8UL)     /* MODE=00, CNF=10 */
#define GPIO_OUTPUT_PP_2MHZ   (0x2UL)     /* MODE=10, CNF=00 */


/* Ghi một nibble cấu hình mà không ảnh hưởng các chân khác trong port. */
static void GPIO_ConfigPin(GPIO_TypeDef *port,
                           uint32_t pin,
                           uint32_t configuration)
{
    volatile uint32_t *configuration_register;
    uint32_t shift;
    uint32_t mask;

    configuration_register = (pin < 8UL) ? &port->CRL : &port->CRH;
    shift = (pin & 7UL) * 4UL;
    mask = 0xFUL << shift;

    *configuration_register =
        (*configuration_register & ~mask) |
        ((configuration & 0xFUL) << shift);
}


void GPIO_Init(void)
{
    uint32_t pin;

    /* Bật clock cho AFIO, GPIOA và GPIOB. */
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN |
                    RCC_APB2ENR_IOPAEN |
                    RCC_APB2ENR_IOPBEN;

    /* Tắt JTAG-DP nhưng giữ SW-DP để PA15 thành GPIO, PA13/PA14 vẫn debug. */
    AFIO->MAPR = (AFIO->MAPR & ~AFIO_MAPR_SWJ_CFG) |
                 AFIO_MAPR_SWJ_CFG_JTAGDISABLE;

    /*
     * PA0..PA7 là input pull-down.
     * ODR=0 chọn điện trở kéo xuống nội, nên input mặc định là LOW.
     */
    GPIOA->BRR = GPIO_INPUT_MASK;
    for (pin = 0UL; pin <= 7UL; pin++)
    {
        GPIO_ConfigPin(GPIOA, pin, GPIO_INPUT_PULLDOWN);
    }
}


uint8_t GPIO_ReadInput(void)
{
    return (uint8_t)(GPIOA->IDR & GPIO_INPUT_MASK);
}


void GPIO_ConfigOutputPin(GPIO_TypeDef *port, uint32_t pin)
{
    GPIO_ConfigPin(port, pin, GPIO_OUTPUT_PP_2MHZ);
}


void GPIO_WriteMasked(GPIO_TypeDef *port, uint32_t mask, uint32_t value)
{
    /* Chỉ thay đổi các bit thuộc mask, không đụng các chân khác của port. */
    port->BRR = mask;
    port->BSRR = value & mask;
}
