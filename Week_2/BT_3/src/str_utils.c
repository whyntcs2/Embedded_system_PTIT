#include "str_utils.h"

void int_to_str(uint16_t num, char *str) {
    char temp[10];
    int i = 0, j = 0;

    if (num == 0) {
        str[0] = '0';
        str[1] = '\0';
        return;
    }

    while (num > 0) {
        temp[i++] = (num % 10) + '0';
        num /= 10;
    }

    // Đảo ngược chuỗi
    while (i > 0) {
        str[j++] = temp[--i];
    }
    str[j] = '\0';
}

void float_to_str(float num, char *str) {
    int int_part = (int)num;
    int dec_part = (int)((num - int_part) * 100); // Lấy 2 chữ số thập phân

    if (dec_part < 0) dec_part = -dec_part;

    char int_str[10];
    char dec_str[10];

    int_to_str(int_part, int_str);
    int_to_str(dec_part, dec_str);

    // Ghép chuỗi: phần nguyên + '.' + phần thập phân
    int i = 0, j = 0;
    while (int_str[i] != '\0') str[j++] = int_str[i++];
    str[j++] = '.';
    
    // Đảm bảo dạng 2 chữ số thập phân (ví dụ .05)
    if (dec_part < 10) str[j++] = '0';
    
    i = 0;
    while (dec_str[i] != '\0') str[j++] = dec_str[i++];
    str[j] = '\0';
}
