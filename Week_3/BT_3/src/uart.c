#include "uart.h"

#define TX_SIZE 300

static char tx_buffer[TX_SIZE];

static uint16_t tx_length = 0;

static volatile uint8_t dma_busy = 0;

void uart_cfg(void)
{
    GPIO_InitTypeDef UART;
    USART_InitTypeDef uart;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |
                           RCC_APB2Periph_USART1,
                           ENABLE);

    UART.GPIO_Pin = GPIO_Pin_9;
    UART.GPIO_Mode = GPIO_Mode_AF_PP;
    UART.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &UART);

    UART.GPIO_Pin = GPIO_Pin_10;
    UART.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &UART);

    uart.USART_BaudRate = 115200;
    uart.USART_WordLength = USART_WordLength_8b;
    uart.USART_StopBits = USART_StopBits_1;
    uart.USART_Parity = USART_Parity_No;
    uart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    uart.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;

    USART_Init(USART1, &uart);

    USART_Cmd(USART1, ENABLE);
}

void uart_dma_cfg(void)
{
    DMA_InitTypeDef dma;
    NVIC_InitTypeDef nvic;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    DMA_DeInit(DMA1_Channel4);

    dma.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR;
    dma.DMA_MemoryBaseAddr = (uint32_t)tx_buffer;
    dma.DMA_DIR = DMA_DIR_PeripheralDST;
    dma.DMA_BufferSize = 1;

    dma.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    dma.DMA_MemoryInc = DMA_MemoryInc_Enable;

    dma.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    dma.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;

    dma.DMA_Mode = DMA_Mode_Normal;
    dma.DMA_Priority = DMA_Priority_Medium;
    dma.DMA_M2M = DMA_M2M_Disable;

    DMA_Init(DMA1_Channel4, &dma);

    DMA_ITConfig(DMA1_Channel4,
                 DMA_IT_TC,
                 ENABLE);

    nvic.NVIC_IRQChannel = DMA1_Channel4_IRQn;
    nvic.NVIC_IRQChannelPreemptionPriority = 1;
    nvic.NVIC_IRQChannelSubPriority = 1;
    nvic.NVIC_IRQChannelCmd = ENABLE;

    NVIC_Init(&nvic);

    USART_DMACmd(USART1,
                 USART_DMAReq_Tx,
                 ENABLE);
}

void uart_prepare_message(uint16_t value)
{
    char prefix[] = "ELE1415-20261-01_nhom1:BTN:";
    char number[6];

    uint16_t i = 0;
    uint16_t j = 0;
    uint16_t len = 0;

    while (prefix[i] != '\0')
    {
        tx_buffer[j++] = prefix[i++];
    }

    if (value == 0)
    {
        tx_buffer[j++] = '0';
    }
    else
    {
        while (value > 0)
        {
            number[len++] = (value % 10) + '0';

            value /= 10;
        }

        while (len > 0)
        {
            tx_buffer[j++] = number[--len];
        }
    }

    tx_buffer[j++] = '\n';
    tx_buffer[j++] = '\r';

    tx_length = j;
}

void uart_dma_send(void)
{
    if (dma_busy)
    {
        return;
    }

    dma_busy = 1;

    DMA_Cmd(DMA1_Channel4, DISABLE);

    DMA_ClearFlag(DMA1_FLAG_GL4);

    DMA1_Channel4->CMAR = (uint32_t)tx_buffer;

    DMA_SetCurrDataCounter(DMA1_Channel4,
                           tx_length);

    DMA_Cmd(DMA1_Channel4, ENABLE);
}

uint8_t uart_is_busy(void)
{
    return dma_busy;
}

void DMA1_Channel4_IRQHandler(void)
{
    if (DMA_GetITStatus(DMA1_IT_TC4) != RESET)
    {
        DMA_ClearITPendingBit(DMA1_IT_TC4);

        DMA_Cmd(DMA1_Channel4, DISABLE);

        dma_busy = 0;
    }
}