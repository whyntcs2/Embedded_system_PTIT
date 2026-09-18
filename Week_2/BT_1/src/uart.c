#include "uart.h"

volatile char rx_buffer[RX_Size];
volatile char tx_buffer[TX_Size];

volatile uint16_t rx_index = 0;
volatile uint16_t tx_index = 0;
volatile uint16_t tx_length = 0;

void uart_cfg(void){
    // Cấu hình GPIO
    GPIO_InitTypeDef UART;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);
    UART.GPIO_Pin = GPIO_Pin_9;
    UART.GPIO_Mode = GPIO_Mode_AF_PP;
    UART.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &UART);

    UART.GPIO_Pin = GPIO_Pin_10;
    UART.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &UART);

    // Cấu hình UART
    USART_InitTypeDef uart;
    uart.USART_BaudRate = 115200;
    uart.USART_WordLength = USART_WordLength_8b;
    uart.USART_StopBits = USART_StopBits_1;
    uart.USART_Parity = USART_Parity_No;
    uart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    uart.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_Init(USART1, &uart);

    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

    // Cấu hình NVIC
    NVIC_InitTypeDef nvic;
    nvic.NVIC_IRQChannel = USART1_IRQn;
    nvic.NVIC_IRQChannelPreemptionPriority = 0;
    nvic.NVIC_IRQChannelSubPriority = 0;
    nvic.NVIC_IRQChannelCmd = ENABLE;

    NVIC_Init(&nvic);

    USART_Cmd(USART1, ENABLE);
}

void prepare_tx_buffer(void)
{
    char prefix[] = "ELE1415-20261-01_nhom1:";
    uint16_t i = 0;
    uint16_t j = 0;

    while (prefix[i] != '\0')
    {
        tx_buffer[j++] = prefix[i++];
    }

    i = 0;
    while (rx_buffer[i] != '\0')
    {
        tx_buffer[j++] = rx_buffer[i++];
    }

    tx_buffer[j++] = '\n';
    tx_buffer[j++] = '\r';

    tx_buffer[j] = '\0';
    tx_length = j;
}

void USART1_IRQHandler(void)
{
    char data;
    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        data = USART_ReceiveData(USART1);
        if (data == '!')
        {
            rx_buffer[rx_index] = '\0';
            prepare_tx_buffer();
            tx_index = 0;
            USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
        }
        else
        {
            if (rx_index < RX_Size - 1)
            {
                rx_buffer[rx_index++] = data;
            }
        }
    }

    if (USART_GetITStatus(USART1, USART_IT_TXE) != RESET)
    {
        if (tx_index < tx_length)
        {
            USART_SendData(USART1, tx_buffer[tx_index++]);
        }
        else
        {
            USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
            rx_index = 0;
            tx_index = 0;
            tx_length = 0;
        }
    }
}