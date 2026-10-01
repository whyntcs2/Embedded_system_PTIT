#ifndef BT3_GPIO_H
#define BT3_GPIO_H

#include "stm32f1xx.h"
#include <stdint.h>

/* Khởi tạo clock, SWD/JTAG và các chân input PA0..PA7. */
void GPIO_Init(void);

/* Đọc 8 bit input PA0..PA7. */
uint8_t GPIO_ReadInput(void);

/* Các primitive dùng bởi module LED để cấu hình và ghi GPIO. */
void GPIO_ConfigOutputPin(GPIO_TypeDef *port, uint32_t pin);
void GPIO_WriteMasked(GPIO_TypeDef *port, uint32_t mask, uint32_t value);

#endif /* BT3_GPIO_H */
