#include "ssd1306.h"
#include "stm32f10x_i2c.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"

static void I2C1_LowLevel_Init(void) {
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    // PB6 - SCL, PB7 - SDA
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    I2C_InitTypeDef I2C_InitStructure;
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_OwnAddress1 = 0x00;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_ClockSpeed = 400000; // 400kHz Fast Mode
    I2C_Init(I2C1, &I2C_InitStructure);

    I2C_Cmd(I2C1, ENABLE);
}

static void SSD1306_WriteByte(uint8_t mode, uint8_t data) {
    while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));

    I2C_GenerateSTART(I2C1, ENABLE);
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

    I2C_Send7bitAddress(I2C1, SSD1306_I2C_ADDR, I2C_Direction_Transmitter);
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

    I2C_SendData(I2C1, mode); // 0x00 cho Command, 0x40 cho Data
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    I2C_SendData(I2C1, data);
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    I2C_GenerateSTOP(I2C1, ENABLE);
}

void SSD1306_Init(void) {
    I2C1_LowLevel_Init();

    // Chuỗi lệnh khởi tạo chuẩn màn hình SSD1306 128x64
    SSD1306_WriteByte(0x00, 0xAE); // Turn off display
    SSD1306_WriteByte(0x00, 0x20); // Set Memory Addressing Mode
    SSD1306_WriteByte(0x00, 0x00); // Horizontal Addressing Mode
    SSD1306_WriteByte(0x00, 0x8D); // Charge Pump command
    SSD1306_WriteByte(0x00, 0x14); // Enable Charge Pump
    SSD1306_WriteByte(0x00, 0xAF); // Turn on display
}

void SSD1306_Clear(void) {
    for (uint16_t i = 0; i < 1024; i++) {
        SSD1306_WriteByte(0x40, 0x00);
    }
}

void SSD1306_DrawBitmap(const uint8_t *bitmap) {
    // Đặt địa chỉ về đầu màn hình (Column 0, Page 0)
    SSD1306_WriteByte(0x00, 0x21); SSD1306_WriteByte(0x00, 0); SSD1306_WriteByte(0x00, 127);
    SSD1306_WriteByte(0x00, 0x22); SSD1306_WriteByte(0x00, 0); SSD1306_WriteByte(0x00, 7);

    // Gửi toàn bộ 1024 bytes mảng ảnh đen trắng
    for (uint16_t i = 0; i < 1024; i++) {
        SSD1306_WriteByte(0x40, bitmap[i]);
    }
}
