#define RCC_BASE        0x40021000UL
#define GPIOA_BASE      0x40010800UL

/* Thanh ghi bật Clock cho PORT A (Bit 2) */
#define RCC_APB2ENR     (*(volatile unsigned int *)(RCC_BASE + 0x18))

/* Thanh ghi cấu hình Chân PA0 -> PA7 */
#define GPIOA_CRL       (*(volatile unsigned int *)(GPIOA_BASE + 0x00))

/* Thanh ghi xuất tín hiệu Output */
#define GPIOA_ODR       (*(volatile unsigned int *)(GPIOA_BASE + 0x0C))

void SystemInit(void) {
    /* Trống - đáp ứng yêu cầu gọi từ startup_stm32f10x_md.s */
}

/* Hàm delay khoảng 1ms (ở tần số xung HSI 8MHz mặc định) */
void delay_ms(volatile unsigned int ms) {
    for (volatile unsigned int i = 0; i < ms; i++) {
        for (volatile unsigned int j = 0; j < 800; j++) {
            __asm__("nop");
        }
    }
}

int main(void) {
    /* 1. Bật Clock cho PORT A (Bit 2 trong RCC_APB2ENR) */
    RCC_APB2ENR |= (1 << 2);

    /* 2. Cấu hình PA0 -> PA4 là Output Push-Pull, tốc độ 2MHz (0x2)
       Xóa 20 bit cấu hình cũ (4 bits/chân * 5 chân = 20 bits) */
    GPIOA_CRL &= ~(0x000FFFFF);
    /* Ghi 0x22222 vào CRL để cài Output 2MHz cho PA0, PA1, PA2, PA3, PA4 */
    GPIOA_CRL |=  (0x00022222);

    /* Mặc định tắt hết tất cả LED ban đầu */
    GPIOA_ODR &= ~(0x1F);

    while (1) {
        /* BẬT DẦN TỪ PA0 -> PA4 (Mỗi LED cách nhau 1s) */
        for (int i = 0; i <= 4; i++) {
            GPIOA_ODR |= (1 << i);   // Bật chân PA[i]
            delay_ms(1000);          // Chờ 1 giây
        }

        /* TẮT DẦN TỪ PA4 -> PA0 (Mỗi LED cách nhau 1s) */
        for (int i = 4; i >= 0; i--) {
            GPIOA_ODR &= ~(1 << i);  // Tắt chân PA[i]
            delay_ms(1000);          // Chờ 1 giây
        }
    }

    return 0;
}
