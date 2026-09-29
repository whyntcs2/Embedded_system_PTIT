#include "stm32f10x.h"
#include "ssd1306.h"

// Mảng ảnh mẫu đen trắng (128x64 px = 1024 bytes)
const uint8_t image_mono_128x64[1024] = {
    [0 ... 511] = 0xAA,  // Nửa trên hiển thị sọc caro ngang 1
    [512 ... 1023] = 0x55 // Nửa dưới hiển thị sọc caro ngang 2
};

int main(void) {
    SSD1306_Init();
    SSD1306_Clear();

    // Hiển thị dữ liệu ảnh đen trắng lên màn hình OLED
    SSD1306_DrawBitmap(image_mono_128x64);

    while (1) {
        // Màn hình hiển thị liên tục
    }
}
