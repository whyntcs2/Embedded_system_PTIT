#include "stm32f10x.h"
#include "adc_temp.h"
#include "usart.h"
#include "str_utils.h"

void Delay_ms(uint32_t ms) {
    uint32_t i, j;
    for (i = 0; i < ms; i++) {
        for (j = 0; j < 0x2AFF; j++);
    }
}

int main(void) {
    uint16_t raw_adc;
    float temp;

    char adc_str[10];
    char temp_str[10];

    USART1_Init(9600);
    ADC1_TempSensor_Init();

    while (1) {
        raw_adc = ADC1_ReadRaw();
        temp = ADC1_CalculateTemp(raw_adc);

        // Chuyển đổi số sang chuỗi thủ công
        int_to_str(raw_adc, adc_str);
        float_to_str(temp, temp_str);

        // Gửi từng phần qua UART
        USART1_SendString("ADC Value: ");
        USART1_SendString(adc_str);
        USART1_SendString(" | Temperature: ");
        USART1_SendString(temp_str);
        USART1_SendString(" C\r\n");

        Delay_ms(1000);
    }
}
