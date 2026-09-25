#ifndef __SSD1306_H
#define __SSD1306_H

#include "stm32f10x.h"

#define SSD1306_I2C_ADDR 0x78 // Địa chỉ I2C 8-bit (0x3C << 1)

void SSD1306_Init(void);
void SSD1306_Clear(void);
void SSD1306_DrawBitmap(const uint8_t *bitmap);

#endif
