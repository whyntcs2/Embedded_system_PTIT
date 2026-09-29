#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_spi.h"
#include "stm32f10x_tim.h"
#include "misc.h"

#include "uart.h"
#include "delay.h"
#include "ff.h"


uint8_t fatfs_read_file(const char *path);
uint8_t fatfs_write_file(const char *path);


int main(void)
{
    static FATFS fs;
    FRESULT res;

    uart_cfg();
    Timer2_cfg();

    while (tx_length != 0);
    uart_send_string_it("\r\n===== NEW BOOT =====\r\n");

    /* Mount FAT32 filesystem */
    res = f_mount(&fs, "0:", 1);

    if (res != FR_OK)
    {
        while (tx_length != 0);
        uart_send_string_it("FATFS MOUNT ERROR\r\n");

        while (1)
        {
        }
    }

    while (tx_length != 0);
    uart_send_string_it("FATFS MOUNT OK\r\n");


    // READ
    fatfs_read_file("0:/READ.TXT");


    /* 
       Test WRITE
       fatfs_write_file("0:/TEST.TXT");
    */


    while (1)
    {
    }
}


// READ
uint8_t fatfs_read_file(const char *path)
{
    static FIL file;
    static char read_buffer[129];

    FRESULT res;
    UINT br;

    res = f_open(&file, path, FA_READ);

    if (res != FR_OK)
    {
        while (tx_length != 0);
        uart_send_string_it("FILE READ OPEN ERROR\r\n");

        return 0;
    }

    while (tx_length != 0);
    uart_send_string_it("FILE READ OPEN OK\r\n");

    while (1)
    {
        res = f_read(&file,
                     read_buffer,
                     128,
                     &br);

        /* Lỗi đọc */
        if (res != FR_OK)
        {
            while (tx_length != 0);
            uart_send_string_it("\r\nFILE READ ERROR\r\n");

            f_close(&file);

            return 0;
        }

        /* br = 0 -> EOF */
        if (br == 0)
        {
            break;
        }

        /*
         * f_read() không tự thêm '\0',
         * nên phải tự thêm để gửi như string qua UART.
         */
        read_buffer[br] = '\0';

        while (tx_length != 0);
        uart_send_string_it(read_buffer);
    }


    res = f_close(&file);

    if (res != FR_OK)
    {
        while (tx_length != 0);
        uart_send_string_it("\r\nFILE READ CLOSE ERROR\r\n");

        return 0;
    }

    while (tx_length != 0);
    uart_send_string_it("\r\nFILE READ CLOSE OK\r\n");

    return 1;
}


// Write file
uint8_t fatfs_write_file(const char *path)
{
    static FIL file;

    FRESULT res;
    UINT bw;

    const char text[] = "Hello from STM32 + FatFs!\r\n";

    res = f_open(&file,
                 path,
                 FA_WRITE | FA_CREATE_ALWAYS);

    if (res != FR_OK)
    {
        while (tx_length != 0);
        uart_send_string_it("FILE WRITE OPEN ERROR\r\n");

        return 0;
    }

    while (tx_length != 0);
    uart_send_string_it("FILE WRITE OPEN OK\r\n");


    res = f_write(&file,
                  text,
                  sizeof(text) - 1,
                  &bw);

    if ((res != FR_OK) ||
        (bw != sizeof(text) - 1))
    {
        while (tx_length != 0);
        uart_send_string_it("FILE WRITE ERROR\r\n");

        f_close(&file);

        return 0;
    }

    while (tx_length != 0);
    uart_send_string_it("FILE WRITE OK\r\n");


    res = f_close(&file);

    if (res != FR_OK)
    {
        while (tx_length != 0);
        uart_send_string_it("FILE WRITE CLOSE ERROR\r\n");

        return 0;
    }

    while (tx_length != 0);
    uart_send_string_it("FILE WRITE CLOSE OK\r\n");

    return 1;
}