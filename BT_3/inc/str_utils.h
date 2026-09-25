#ifndef __STR_UTILS_H
#define __STR_UTILS_H

#include "stm32f10x.h"

// Chuyển số nguyên thành chuỗi
void int_to_str(uint16_t num, char *str);

// Chuyển số thực float (lấy 2 chữ số thập phân) thành chuỗi
void float_to_str(float num, char *str);

#endif
