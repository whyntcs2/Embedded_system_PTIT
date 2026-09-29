#ifndef __SD_CARD_H
#define __SD_CARD_H

#include "stm32f10x.h"

//driver
uint8_t SD_Card_Init(void);

uint8_t sd_read_a_block(uint32_t block,
                        uint8_t *buffer);

uint8_t sd_write_a_block(uint32_t block,
                         const uint8_t *buffer);

//debug
uint8_t sd_test_write_read(uint32_t block);

#endif