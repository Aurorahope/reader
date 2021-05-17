//
// Created by lenovo on 2021/4/8.
//

#ifndef READER_FLASH_H
#define READER_FLASH_H

#include "stm32f1xx_hal.h"

void FLASH_EEPROM_Write(uint32_t n);
uint32_t FLASH_EEPROM_Read(void);

#endif //READER_FLASH_H
