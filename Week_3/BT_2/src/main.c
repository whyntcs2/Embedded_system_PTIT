#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_spi.h"
#include "uart.h"

#define SD_CS_LOW()   GPIO_ResetBits(GPIOA, GPIO_Pin_4)
#define SD_CS_HIGH()  GPIO_SetBits(GPIOA, GPIO_Pin_4)

void gpio_cfg(void);
void spi_cfg(void);
uint8_t spi_transfer(uint8_t data);
uint8_t sd_send_command(uint8_t cmd, uint32_t arg, uint8_t crc);
uint8_t sd_read_block(uint32_t block, uint8_t *buffer);
uint8_t sd_write_block(uint32_t block, const uint8_t *buffer);

int main(void)
{
    uint8_t response;
    uint8_t r7[4];
    uint16_t timeout = 1000;

    gpio_cfg();
    spi_cfg();
    uart_cfg();

    /* SD Card chưa được chọn */
    SD_CS_HIGH();

    /* Gửi >= 74 xung clock */
    for (uint8_t i = 0; i < 10; i++)
    {
        spi_transfer(0xFF);
    }

    /* ================= CMD0 ================= */

    response = sd_send_command(0, 0x00000000, 0x95);

    SD_CS_HIGH();
    spi_transfer(0xFF);

    if (response != 0x01)
    {
        uart_send_string_it("CMD0 ERROR\r\n");

        while (1)
        {
        }
    }

    while (tx_length != 0);
    uart_send_string_it("CMD0 OK\r\n");

    /* ================= CMD8 ================= */

    response = sd_send_command(8, 0x000001AA, 0x87);

    if (response != 0x01)
    {
        SD_CS_HIGH();
        spi_transfer(0xFF);

        while (tx_length != 0);
        uart_send_string_it("CMD8 ERROR\r\n");

        while (1)
        {
        }
    }

    /* CMD8 trả thêm 4 byte R7 */
    r7[0] = spi_transfer(0xFF);
    r7[1] = spi_transfer(0xFF);
    r7[2] = spi_transfer(0xFF);
    r7[3] = spi_transfer(0xFF);

    SD_CS_HIGH();
    spi_transfer(0xFF);

    if ((r7[2] != 0x01) || (r7[3] != 0xAA))
    {
        while (tx_length != 0);
        uart_send_string_it("CMD8 R7 ERROR\r\n");

        while (1)
        {
        }
    }

    while (tx_length != 0);
    uart_send_string_it("CMD8 OK\r\n");

    /* ============== CMD55 + ACMD41 ============== */

    do
    {
        /* CMD55 */
        response = sd_send_command(55, 0x00000000, 0xFF);

        SD_CS_HIGH();
        spi_transfer(0xFF);

        if (response > 0x01)
        {
            while (tx_length != 0);
            uart_send_string_it("CMD55 ERROR\r\n");

            break;
        }

        /* ACMD41 - HCS = 1 */
        response = sd_send_command(41, 0x40000000, 0xFF);

        SD_CS_HIGH();
        spi_transfer(0xFF);

        timeout--;

    } while ((response != 0x00) && (timeout > 0));

    if (response == 0x00)
    {
        while (tx_length != 0);
        uart_send_string_it("ACMD41 OK\r\n");
    }
    else
    {
        while (tx_length != 0);
        uart_send_string_it("ACMD41 ERROR\r\n");
    }
    uint8_t ocr[4];

    response = sd_send_command(58, 0x00000000, 0xFF);

    if (response == 0x00)
    {
        ocr[0] = spi_transfer(0xFF);
        ocr[1] = spi_transfer(0xFF);
        ocr[2] = spi_transfer(0xFF);
        ocr[3] = spi_transfer(0xFF);

        SD_CS_HIGH();
        spi_transfer(0xFF);

        while (tx_length != 0);

        if (ocr[0] & 0x40)
        {
            uart_send_string_it("CMD58 OK - SDHC/SDXC\r\n");
        }
        else
        {
            uart_send_string_it("CMD58 OK - SDSC\r\n");
        }
    }
    else
    {
        SD_CS_HIGH();
        spi_transfer(0xFF);

        while (tx_length != 0);
        uart_send_string_it("CMD58 ERROR\r\n");
    }
    uint8_t buffer[512];

    if (sd_read_block(0, buffer))
    {
        while (tx_length != 0);
        uart_send_string_it("READ BLOCK 0 OK\r\n");
    }
    else
    {
        while (tx_length != 0);
        uart_send_string_it("READ BLOCK 0 ERROR\r\n");
    }

    while (1)
    {
    }
}

void gpio_cfg(void){
    GPIO_InitTypeDef GPIO;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE);
    GPIO.GPIO_Pin = GPIO_Pin_4;
    GPIO.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO);

    GPIO.GPIO_Pin = GPIO_Pin_6;
    GPIO.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO);

    GPIO.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;
    GPIO.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO);

    GPIO_SetBits(GPIOA, GPIO_Pin_4);
}

void spi_cfg(void){
    SPI_InitTypeDef SD_Card;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);

    SD_Card.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SD_Card.SPI_Mode = SPI_Mode_Master;
    SD_Card.SPI_DataSize = SPI_DataSize_8b;

    SD_Card.SPI_CPOL = SPI_CPOL_Low;
    SD_Card.SPI_CPHA = SPI_CPHA_1Edge;
    SD_Card.SPI_NSS = SPI_NSS_Soft;
    SD_Card.SPI_FirstBit = SPI_FirstBit_MSB;
    SD_Card.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
    SD_Card.SPI_CRCPolynomial = 7;

    SPI_Init(SPI1, &SD_Card);
    SPI_Cmd(SPI1, ENABLE);
}

uint8_t spi_transfer(uint8_t data){
    while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
    SPI_I2S_SendData(SPI1, data);
    while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE)== RESET);
    return (uint8_t) SPI_I2S_ReceiveData(SPI1);
}

uint8_t sd_send_command(uint8_t cmd, uint32_t arg, uint8_t crc)
{
    uint8_t response;

    SD_CS_LOW();

    spi_transfer(0x40 | cmd);
    spi_transfer((uint8_t)(arg >> 24));
    spi_transfer((uint8_t)(arg >> 16));
    spi_transfer((uint8_t)(arg >> 8));
    spi_transfer((uint8_t)arg);
    spi_transfer(crc);

    for (uint8_t i = 0; i < 8; i++)
    {
        response = spi_transfer(0xFF);

        if ((response & 0x80) == 0)
        {
            return response;
        }
    }

    return 0xFF;
}

uint8_t sd_read_block(uint32_t block, uint8_t *buffer)
{
    uint8_t response;
    uint8_t token;
    uint16_t timeout = 0xFFFF;

    response = sd_send_command(17, block, 0xFF);

    if (response != 0x00)
    {
        SD_CS_HIGH();
        spi_transfer(0xFF);
        return 0;
    }

    /* Chờ data token 0xFE */
    do
    {
        token = spi_transfer(0xFF);
        timeout--;

    } while ((token != 0xFE) && (timeout > 0));

    if (token != 0xFE)
    {
        SD_CS_HIGH();
        spi_transfer(0xFF);
        return 0;
    }

    /* Đọc 512 byte */
    for (uint16_t i = 0; i < 512; i++)
    {
        buffer[i] = spi_transfer(0xFF);
    }

    /* Bỏ 2 byte CRC */
    spi_transfer(0xFF);
    spi_transfer(0xFF);

    SD_CS_HIGH();

    /* Extra clock */
    spi_transfer(0xFF);

    return 1;
}

uint8_t sd_write_block(uint32_t block, const uint8_t *buffer)
{
    uint8_t response;
    uint8_t data_response;
    uint16_t timeout = 0xFFFF;

    response = sd_send_command(24, block, 0xFF);

    if (response != 0x00)
    {
        SD_CS_HIGH();
        spi_transfer(0xFF);
        return 0;
    }

    /* Một byte dummy trước data token */
    spi_transfer(0xFF);

    /* Start Block Token */
    spi_transfer(0xFE);

    /* Gửi 512 byte */
    for (uint16_t i = 0; i < 512; i++)
    {
        spi_transfer(buffer[i]);
    }

    /* CRC dummy */
    spi_transfer(0xFF);
    spi_transfer(0xFF);

    /* Đọc Data Response Token */
    data_response = spi_transfer(0xFF);

    if ((data_response & 0x1F) != 0x05)
    {
        SD_CS_HIGH();
        spi_transfer(0xFF);
        return 0;
    }

    /* Chờ SD Card ghi xong.
       Trong lúc busy, MISO trả 0x00 */
    while ((spi_transfer(0xFF) == 0x00) && (timeout > 0))
    {
        timeout--;
    }

    SD_CS_HIGH();
    spi_transfer(0xFF);

    if (timeout == 0)
    {
        return 0;
    }

    return 1;
}