#ifndef BT3_LED_H
#define BT3_LED_H

#include <stdint.h>

/* Khởi tạo các chân LED PA8..PA12, PB13, PB12 và PA15. */
void LED_Init(void);

/*
 * Ghi mức điện ra dãy LED.
 * Bit 0..7 tương ứng PA8..PA12, PB13, PB12, PA15.
 * Đây là mức vật lý: bit 1 = output HIGH (LED dương chung tắt),
 * bit 0 = output LOW (LED dương chung sáng).
 */
void LED_Write(uint8_t output_levels);

#endif /* BT3_LED_H */
