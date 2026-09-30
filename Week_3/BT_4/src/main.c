#include "stm32f1xx.h"
#include <stdint.h>

#define BUFFER_SIZE  100U
#define HALF_SIZE     50U

#define FLAG_HT      0x01U
#define FLAG_TC      0x02U

/* Mảng này nằm trong RAM và được DMA tự động ghi dữ liệu ADC vào. */
static volatile uint16_t adc_buffer[BUFFER_SIZE];

/* Biến này báo cho chương trình chính biết nửa buffer nào đã hoàn thành. */
static volatile uint32_t dma_ready = 0;

/* Các biến này dùng để kiểm tra hoạt động của DMA khi debug. */
volatile uint32_t ht_count = 0;
volatile uint32_t tc_count = 0;
volatile uint32_t dma_error = 0;
volatile uint32_t overrun_count = 0;


/* Khởi tạo USART1 trên chân PA9 với baudrate 115200. */
void UART1_Init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

    /* PA9 được cấu hình là Alternate Function Push-Pull 50 MHz. */
    GPIOA->CRH &= ~(0xFUL << 4);
    GPIOA->CRH |=  (0xBUL << 4);

    /* PCLK2 là 8 MHz nên BRR bằng 0x45 cho baudrate gần 115200. */
    USART1->BRR = 0x45U;

    /* Bật USART1 và bộ truyền dữ liệu. */
    USART1->CR1 = USART_CR1_UE | USART_CR1_TE;
}


/* Hàm này gửi một ký tự qua USART1. */
void UART1_SendChar(char c)
{
    while (!(USART1->SR & USART_SR_TXE))
    {
    }

    USART1->DR = (uint8_t)c;
}


/* Hàm này gửi một chuỗi ký tự qua USART1. */
void UART1_SendString(const char *str)
{
    while (*str != '\0')
    {
        UART1_SendChar(*str);
        str++;
    }
}


/* Hàm này chuyển số nguyên thành ký tự rồi gửi qua USART1. */
void UART1_SendNumber(uint16_t value)
{
    char buffer[5];
    uint8_t index = 0;

    if (value == 0U)
    {
        UART1_SendChar('0');
        return;
    }

    while (value > 0U)
    {
        buffer[index] = (char)('0' + (value % 10U));
        value /= 10U;
        index++;
    }

    while (index > 0U)
    {
        index--;
        UART1_SendChar(buffer[index]);
    }
}


/* Hàm này gửi một đoạn buffer ADC và ngăn cách các mẫu bằng dấu phẩy. */
void UART1_SendBuffer(
    const volatile uint16_t *buffer,
    uint32_t length
)
{
    uint32_t i;

    for (i = 0; i < length; i++)
    {
        UART1_SendNumber(buffer[i]);

        if (i < (length - 1U))
        {
            UART1_SendChar(',');
        }
    }

    UART1_SendString("\r\n");
}


/* PA0 được cấu hình ở chế độ Analog Input để dùng ADC1 Channel 0. */
void ADC_GPIO_Init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

    GPIOA->CRL &= ~(0xFUL << 0);
}


/* DMA1 Channel 1 tự chuyển dữ liệu từ ADC1 sang adc_buffer trong RAM. */
void DMA1_ADC_Init(void)
{
    RCC->AHBENR |= RCC_AHBENR_DMA1EN;

    /* DMA phải được tắt trước khi thay đổi cấu hình. */
    DMA1_Channel1->CCR = 0;

    /* Xóa các cờ ngắt cũ của DMA1 Channel 1. */
    DMA1->IFCR = DMA_IFCR_CGIF1;

    /* Nguồn dữ liệu của DMA là thanh ghi dữ liệu ADC1. */
    DMA1_Channel1->CPAR = (uint32_t)&ADC1->DR;

    /* Đích dữ liệu của DMA là mảng adc_buffer trong RAM. */
    DMA1_Channel1->CMAR = (uint32_t)adc_buffer;

    /* DMA sẽ thực hiện 100 lần truyền trước khi hoàn thành một chu kỳ. */
    DMA1_Channel1->CNDTR = BUFFER_SIZE;

    /*
     * DMA truyền dữ liệu từ Peripheral sang Memory.
     * Mỗi mẫu ADC và mỗi phần tử RAM đều có kích thước 16 bit.
     * Địa chỉ RAM tự tăng sau mỗi mẫu.
     * DMA chạy ở chế độ Circular.
     * Ngắt HT xảy ra sau 50 mẫu và ngắt TC xảy ra sau 100 mẫu.
     */
    DMA1_Channel1->CCR =
          DMA_CCR_PSIZE_0
        | DMA_CCR_MSIZE_0
        | DMA_CCR_MINC
        | DMA_CCR_CIRC
        | DMA_CCR_PL_0
        | DMA_CCR_HTIE
        | DMA_CCR_TCIE
        | DMA_CCR_TEIE;

    NVIC_SetPriority(DMA1_Channel1_IRQn, 1);
    NVIC_EnableIRQ(DMA1_Channel1_IRQn);

    /* Bật DMA1 Channel 1 sau khi đã cấu hình xong. */
    DMA1_Channel1->CCR |= DMA_CCR_EN;
}


/* ADC1 đọc Channel 0 trên PA0 và được kích bởi TIM3 TRGO. */
void ADC1_Init(void)
{
    uint32_t i;

    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

    /* ADC chạy với clock 4 MHz khi PCLK2 bằng 8 MHz. */
    RCC->CFGR &= ~RCC_CFGR_ADCPRE;

    ADC1->CR1 = 0;
    ADC1->CR2 = 0;

    /* ADC chỉ thực hiện một conversion trong mỗi sequence. */
    ADC1->SQR1 = 0;
    ADC1->SQR2 = 0;

    /* Rank đầu tiên sử dụng ADC Channel 0. */
    ADC1->SQR3 = 0;

    /* Channel 0 sử dụng thời gian lấy mẫu 239.5 chu kỳ ADC. */
    ADC1->SMPR2 &= ~(0x7UL << 0);
    ADC1->SMPR2 |=  (0x7UL << 0);

    /* Bật ADC1. */
    ADC1->CR2 |= ADC_CR2_ADON;

    /* Chờ ADC ổn định trước khi calibration. */
    for (i = 0; i < 1000U; i++)
    {
        __NOP();
    }

    /* Reset mạch calibration của ADC. */
    ADC1->CR2 |= ADC_CR2_RSTCAL;

    while (ADC1->CR2 & ADC_CR2_RSTCAL)
    {
    }

    /* Thực hiện calibration cho ADC. */
    ADC1->CR2 |= ADC_CR2_CAL;

    while (ADC1->CR2 & ADC_CR2_CAL)
    {
    }

    /*
     * EXTSEL bằng 100 chọn TIM3 TRGO làm nguồn trigger.
     * EXTTRIG cho phép ADC nhận trigger bên ngoài.
     * DMA cho phép ADC tạo yêu cầu truyền dữ liệu tới DMA.
     */
    ADC1->CR2 |=
          ADC_CR2_EXTSEL_2
        | ADC_CR2_EXTTRIG
        | ADC_CR2_DMA;
}


/* TIM3 tạo Update Event với tần số 100 Hz và đưa sự kiện này ra TRGO. */
void TIM3_Init(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

    TIM3->CR1 = 0;
    TIM3->CR2 = 0;

    /*
     * Clock TIM3 là 8 MHz.
     * PSC bằng 7999 làm clock bộ đếm còn 1000 Hz.
     * ARR bằng 9 tạo Update Event với tần số 100 Hz.
     */
    TIM3->PSC = 7999U;
    TIM3->ARR = 9U;

    TIM3->CNT = 0;

    /* Tạo Update Event để cập nhật PSC và ARR. */
    TIM3->EGR = TIM_EGR_UG;

    /* Xóa cờ Update Event vừa được tạo khi khởi tạo. */
    TIM3->SR = 0;

    /* MMS bằng 010 chọn Update Event làm tín hiệu TRGO. */
    TIM3->CR2 &= ~TIM_CR2_MMS;
    TIM3->CR2 |= TIM_CR2_MMS_1;
}


/* Hàm ngắt này xử lý các sự kiện của DMA1 Channel 1. */
void DMA1_Channel1_IRQHandler(void)
{
    uint32_t status;

    status = DMA1->ISR;

    /* Nếu DMA gặp lỗi truyền dữ liệu thì tăng biến dma_error. */
    if (status & DMA_ISR_TEIF1)
    {
        DMA1->IFCR = DMA_IFCR_CTEIF1;
        dma_error++;
    }

    /*
     * HT xảy ra khi DMA đã ghi xong adc_buffer[0] đến adc_buffer[49].
     * Nửa đầu buffer lúc này đã an toàn để chương trình chính sử dụng.
     */
    if (status & DMA_ISR_HTIF1)
    {
        DMA1->IFCR = DMA_IFCR_CHTIF1;

        ht_count++;

        if (dma_ready & FLAG_HT)
        {
            overrun_count++;
        }

        dma_ready |= FLAG_HT;
    }

    /*
     * TC xảy ra khi DMA đã ghi xong adc_buffer[50] đến adc_buffer[99].
     * DMA đã hoàn thành 100 mẫu và sẽ quay lại đầu buffer do Circular Mode.
     */
    if (status & DMA_ISR_TCIF1)
    {
        DMA1->IFCR = DMA_IFCR_CTCIF1;

        tc_count++;

        if (dma_ready & FLAG_TC)
        {
            overrun_count++;
        }

        dma_ready |= FLAG_TC;
    }
}


int main(void)
{
    uint32_t ready;

    UART1_Init();
    ADC_GPIO_Init();
    DMA1_ADC_Init();
    ADC1_Init();
    TIM3_Init();

    UART1_SendString("\r\nSTM32 ADC TIM3 DMA UART\r\n");
    UART1_SendString("Sampling Frequency = 100 Hz\r\n");
    UART1_SendString("Buffer = 100 samples\r\n");
    UART1_SendString("HT = 50 samples, TC = 100 samples\r\n\r\n");

    /* TIM3 được bật sau cùng để ADC và DMA đã sẵn sàng trước khi lấy mẫu. */
    TIM3->CR1 |= TIM_CR1_CEN;

    while (1)
    {
        /*
         * Tạm khóa ngắt để lấy dma_ready rồi xóa cờ mà không bị ISR
         * thay đổi biến này trong cùng thời điểm.
         */
        __disable_irq();

        ready = dma_ready;
        dma_ready = 0;

        __enable_irq();

        /*
         * Khi HT xảy ra, 50 mẫu đầu đã hoàn thành.
         * Chương trình gửi adc_buffer[0] đến adc_buffer[49].
         */
        if (ready & FLAG_HT)
        {
            UART1_SendString("HT:");

            UART1_SendBuffer(
                &adc_buffer[0],
                HALF_SIZE
            );
        }

        /*
         * Khi TC xảy ra, 50 mẫu sau đã hoàn thành.
         * Chương trình gửi adc_buffer[50] đến adc_buffer[99].
         */
        if (ready & FLAG_TC)
        {
            UART1_SendString("TC:");

            UART1_SendBuffer(
                &adc_buffer[HALF_SIZE],
                HALF_SIZE
            );
        }
    }
}