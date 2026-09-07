/* Địa chỉ các thanh ghi phần cứng trên STM32F103 */
#define RCC_BASE        0x40021000UL
#define GPIOC_BASE      0x40011000UL

/* Thanh ghi bật Clock cho GPIOC (Bit 4) */
#define RCC_APB2ENR     (*(volatile unsigned int *)(RCC_BASE + 0x18))

/* Thanh ghi cấu hình Chân PC8 -> PC15 */
#define GPIOC_CRH       (*(volatile unsigned int *)(GPIOC_BASE + 0x04))

/* Thanh ghi điều khiển xuất tín hiệu Output */
#define GPIOC_ODR       (*(volatile unsigned int *)(GPIOC_BASE + 0x0C))

void SystemInit(void){}

/* Hàm delay đơn giản */
void delay_ms(volatile unsigned int ms) {
    for (volatile unsigned int i = 0; i < ms * 1000; i++) {
        __asm__("nop");
    }
}

int main(void) {
    /* 1. Bật Clock cho PORT C (Bit 4 trong RCC_APB2ENR) */
    RCC_APB2ENR |= (1 << 4);

    /* 2. Cấu hình chân PC13 là Output Push-Pull, tốc độ 2MHz
       Xóa 4-bit cấu hình cũ của PC13 (Bits 20..23) */
    GPIOC_CRH &= ~(0xF << 20);
    /* Ghi giá trị 0x2 (0010b) vào Bits 20..23 */
    GPIOC_CRH |=  (0x2 << 20);

    /* 3. Vòng lặp chớp tắt LED */
    while (1) {
        /* Đảo trạng thái Bit 13 trên thanh ghi ODR */
        GPIOC_ODR ^= (1 << 13);
        
        delay_ms(500);
    }

    return 0;
}
