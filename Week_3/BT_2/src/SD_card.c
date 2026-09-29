#include "SD_card.h"
#include "Delay.h"
#include "uart.h"

static void gpio_cfg(void);
static void spi_cfg(void);
static void spi_set_fast(void);

static uint8_t spi_transfer(uint8_t data);
static uint8_t sd_send_command(uint8_t cmd,
                               uint32_t arg,
                               uint8_t crc);
static uint8_t sd_wait_ready(uint32_t timeout_ms);

static char hex_char(uint8_t value);
static void uart_print_r7(void);

static uint8_t r7[4];
static uint8_t ocr[4];

#define SD_CS_LOW()   GPIO_ResetBits(GPIOA, GPIO_Pin_4)
#define SD_CS_HIGH()  GPIO_SetBits(GPIOA, GPIO_Pin_4)

static void gpio_cfg(void){
    GPIO_InitTypeDef GPIO;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE);
    GPIO_SetBits(GPIOA, GPIO_Pin_4);
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

   
}

static void spi_cfg(void){
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

static uint8_t spi_transfer(uint8_t data){
    while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
    SPI_I2S_SendData(SPI1, data);
    while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE)== RESET);
    return (uint8_t) SPI_I2S_ReceiveData(SPI1);
}

static uint8_t sd_send_command(uint8_t cmd, uint32_t arg, uint8_t crc)
{
    uint8_t response;

    SD_CS_LOW();

    if (!sd_wait_ready(500))
    {
        SD_CS_HIGH();
        spi_transfer(0xFF);
        return 0xFF;
    }

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

uint8_t sd_read_a_block(uint32_t block, uint8_t *buffer)
{
    uint8_t response;
    uint8_t token;
    uint32_t start;

    response = sd_send_command(17, block, 0xFF);

    if (response != 0x00)
    {
        SD_CS_HIGH();
        spi_transfer(0xFF);

        while (tx_length != 0);
        uart_send_string_it("READ ERROR: CMD17 R1\r\n");

        return 0;
    }

    start = ms_ticks;

    do
    {
        token = spi_transfer(0xFF);

    } while ((token != 0xFE) &&
             ((ms_ticks - start) < 150));

    if (token != 0xFE)
    {
        SD_CS_HIGH();
        spi_transfer(0xFF);

        while (tx_length != 0);
        uart_send_string_it("READ ERROR: TOKEN TIMEOUT\r\n");

        return 0;
    }

    for (uint16_t i = 0; i < 512; i++)
    {
        buffer[i] = spi_transfer(0xFF);
    }

    spi_transfer(0xFF);
    spi_transfer(0xFF);

    SD_CS_HIGH();
    spi_transfer(0xFF);

    return 1;
}

uint8_t sd_write_a_block(uint32_t block, const uint8_t *buffer)
{
    uint8_t response;
    uint8_t data_response;

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

    /* 0x05 = Data accepted */
    if ((data_response & 0x1F) != 0x05)
    {
        SD_CS_HIGH();
        spi_transfer(0xFF);
        return 0;
    }

    /* Chờ card ghi xong */
    if (!sd_wait_ready(250))
    {
        SD_CS_HIGH();
        spi_transfer(0xFF);
        return 0;
    }

    SD_CS_HIGH();

    /* Extra 8 clocks */
    spi_transfer(0xFF);

    return 1;
}

uint8_t SD_Card_Init(void)
{
    uint8_t response = 0xFF;
    uint32_t start;
    gpio_cfg();
    spi_cfg();

    /* =========================
       1. Cấp >= 74 clock
       ========================= */
    SD_CS_HIGH();

    for (uint8_t i = 0; i < 10; i++)
    {
        spi_transfer(0xFF);
    }

    /* =========================
       2. CMD0
       ========================= */
    start = ms_ticks;

    do
    {
        response = sd_send_command(0, 0x00000000, 0x95);

        SD_CS_HIGH();
        spi_transfer(0xFF);

        if (response == 0x01)
        {
            break;
        }

    } while ((ms_ticks - start) < 1000);

    if (response != 0x01)
    {
        while (tx_length != 0);
        uart_send_string_it("CMD0 ERROR\r\n");

        return 0;
    }

    while (tx_length != 0);
    uart_send_string_it("CMD0 OK\r\n");


    /* =========================
       3. CMD8
       ========================= */
    response = sd_send_command(8, 0x000001AA, 0x87);

    if (response != 0x01)
    {
        SD_CS_HIGH();
        spi_transfer(0xFF);

        while (tx_length != 0);
        uart_send_string_it("CMD8 ERROR\r\n");

        return 0;
    }

    r7[0] = spi_transfer(0xFF);
    r7[1] = spi_transfer(0xFF);
    r7[2] = spi_transfer(0xFF);
    r7[3] = spi_transfer(0xFF);

    SD_CS_HIGH();
    spi_transfer(0xFF);

    if ((r7[2] != 0x01) ||
        (r7[3] != 0xAA))
    {
        uart_print_r7();

        while (tx_length != 0);
        uart_send_string_it("CMD8 R7 ERROR\r\n");

        return 0;
    }

    while (tx_length != 0);
    uart_send_string_it("CMD8 OK\r\n");


    /* =========================
       4. CMD55 + ACMD41
       ========================= */
    start = ms_ticks;

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

            return 0;
        }

        /* ACMD41 - HCS = 1 */
        response = sd_send_command(41, 0x40000000, 0xFF);

        SD_CS_HIGH();
        spi_transfer(0xFF);

    } while ((response != 0x00) &&
             ((ms_ticks - start) < 1000));

    if (response != 0x00)
    {
        while (tx_length != 0);
        uart_send_string_it("ACMD41 ERROR\r\n");

        return 0;
    }

    while (tx_length != 0);
    uart_send_string_it("ACMD41 OK\r\n");


    /* =========================
       5. CMD58
       ========================= */
    response = sd_send_command(58, 0x00000000, 0xFF);

    if (response != 0x00)
    {
        SD_CS_HIGH();
        spi_transfer(0xFF);

        while (tx_length != 0);
        uart_send_string_it("CMD58 ERROR\r\n");

        return 0;
    }

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
    while (tx_length != 0);

    /* Sau initialization mới tăng SPI lên 18 MHz */
    spi_set_fast();

    return 1;
}

static uint8_t sd_wait_ready(uint32_t timeout_ms)
{
    uint32_t start = ms_ticks;
    do
    {
        if(spi_transfer(0xFF) == 0xFF)
        {
            return 1;
        }
    } while((ms_ticks - start) < timeout_ms);
    return 0;
}

uint8_t sd_test_write_read(uint32_t block)
{
    static uint8_t backup_buffer[512];
    static uint8_t write_buffer[512];
    static uint8_t read_buffer[512];

    uint8_t verify_ok = 1;

    /* 1. Đọc dữ liệu gốc để backup */
    if (!sd_read_a_block(block, backup_buffer))
    {
        while (tx_length != 0);
        uart_send_string_it("TEST: BACKUP READ ERROR\r\n");

        return 0;
    }

    while (tx_length != 0);
    uart_send_string_it("TEST: BACKUP READ OK\r\n");

    /* 2. Tạo dữ liệu test */
    for (uint16_t i = 0; i < 512; i++)
    {
        write_buffer[i] = (uint8_t)(i ^ 0xA5);
    }

    /* 3. Ghi dữ liệu test */
    if (!sd_write_a_block(block, write_buffer))
    {
        while (tx_length != 0);
        uart_send_string_it("TEST: WRITE ERROR\r\n");

        return 0;
    }

    while (tx_length != 0);
    uart_send_string_it("TEST: WRITE OK\r\n");

    /* 4. Đọc lại */
    if (!sd_read_a_block(block, read_buffer))
    {
        while (tx_length != 0);
        uart_send_string_it("TEST: READ BACK ERROR\r\n");

        /* Cố gắng phục hồi dữ liệu gốc */
        sd_write_a_block(block, backup_buffer);

        return 0;
    }

    while (tx_length != 0);
    uart_send_string_it("TEST: READ BACK OK\r\n");

    /* 5. So sánh 512 byte */
    for (uint16_t i = 0; i < 512; i++)
    {
        if (read_buffer[i] != write_buffer[i])
        {
            verify_ok = 0;
            break;
        }
    }

    if (verify_ok)
    {
        while (tx_length != 0);
        uart_send_string_it("TEST: VERIFY OK\r\n");
    }
    else
    {
        while (tx_length != 0);
        uart_send_string_it("TEST: VERIFY ERROR\r\n");
    }

    /* 6. Phục hồi sector ban đầu */
    if (!sd_write_a_block(block, backup_buffer))
    {
        while (tx_length != 0);
        uart_send_string_it("TEST: RESTORE ERROR\r\n");

        return 0;
    }

    while (tx_length != 0);
    uart_send_string_it("TEST: RESTORE OK\r\n");

    return verify_ok;
}

static char hex_char(uint8_t value)
{
    if (value < 10)
    {
        return '0' + value;
    }

    return 'A' + (value - 10);
}

static void uart_print_r7(void)
{
    char msg[] = "R7 = 00 00 00 00\r\n";

    msg[5]  = hex_char((r7[0] >> 4) & 0x0F);
    msg[6]  = hex_char(r7[0] & 0x0F);

    msg[8]  = hex_char((r7[1] >> 4) & 0x0F);
    msg[9]  = hex_char(r7[1] & 0x0F);

    msg[11] = hex_char((r7[2] >> 4) & 0x0F);
    msg[12] = hex_char(r7[2] & 0x0F);

    msg[14] = hex_char((r7[3] >> 4) & 0x0F);
    msg[15] = hex_char(r7[3] & 0x0F);

    while (tx_length != 0);
    uart_send_string_it(msg);
}

static void spi_set_fast(void)
{
    SPI_InitTypeDef SPI_InitStructure;

    /* Chờ SPI truyền xong */
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET);

    /* Disable trước khi cấu hình lại */
    SPI_Cmd(SPI1, DISABLE);

    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;

    /* 72 MHz / 4 = 18 MHz */
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;

    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;

    SPI_Init(SPI1, &SPI_InitStructure);

    SPI_Cmd(SPI1, ENABLE);
}