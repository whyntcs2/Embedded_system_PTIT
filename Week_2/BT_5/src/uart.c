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


void command(void)
{
    if (rx_buffer[0] == 'O' && rx_buffer[1] == 'N' && rx_buffer[2] == '\0'){
        ledstate = 1;
        change_duty(pwmstate);
    }

    else if (rx_buffer[0] == 'O' && rx_buffer[1] == 'F' && rx_buffer[2] == 'F' && rx_buffer[3] == '\0'){
        ledstate = 0;
        change_duty(0);
    }

    else if (rx_buffer[0] == 'P' && rx_buffer[1] == 'W' && rx_buffer[2] == 'M' && rx_buffer[3] == ':'){
        uint16_t value = 0;
        uint8_t i = 4;
        while (rx_buffer[i] >= '0' && rx_buffer[i] <= '9'){
            value = value * 10 + (rx_buffer[i] - '0');
            i++;
        }
        if (i > 4 && rx_buffer[i] == '%' && rx_buffer[i + 1] == '\0' && value <= 100){
            pwmstate = value;
            if (ledstate == 1){
                change_duty(pwmstate);
            }
        }
    }
    else if (rx_buffer[0] == 'S' && rx_buffer[1] == 't' && rx_buffer[2] == 'a' && rx_buffer[3] == 't' && rx_buffer[4] == 'u' && rx_buffer[5] == 's' && rx_buffer[6] == '\0') {
        prepare_status();
        tx_index = 0;
        USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
    }
}

void prepare_status(void)
{
    uint16_t j = 0;
    tx_buffer[j++] = 'L';
    tx_buffer[j++] = 'E';
    tx_buffer[j++] = 'D';
    tx_buffer[j++] = ':';

    if (ledstate == 1)
    {
        tx_buffer[j++] = 'O';
        tx_buffer[j++] = 'N';
    }
    else
    {
        tx_buffer[j++] = 'O';
        tx_buffer[j++] = 'F';
        tx_buffer[j++] = 'F';
    }

    tx_buffer[j++] = ' ';

    tx_buffer[j++] = 'P';
    tx_buffer[j++] = 'W';
    tx_buffer[j++] = 'M';
    tx_buffer[j++] = ':';

    if (pwmstate == 100)
    {
        tx_buffer[j++] = '1';
        tx_buffer[j++] = '0';
        tx_buffer[j++] = '0';
    }
    else if (pwmstate >= 10)
    {
        tx_buffer[j++] = (pwmstate / 10) + '0';
        tx_buffer[j++] = (pwmstate % 10) + '0';
    }
    else
    {
        tx_buffer[j++] = pwmstate + '0';
    }

    tx_buffer[j++] = '%';

    tx_buffer[j++] = '\r';
    tx_buffer[j++] = '\n';

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
            command();
            rx_index = 0;
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
            tx_index = 0;
            tx_length = 0;
        }
    }
}