//
// Created by lenovo on 2021/4/8.
//

#include "flash.h"
#include <string.h>

void FLASH_EEPROM_Write(uint32_t n)
{
    HAL_FLASH_Unlock();     //解锁
    uint32_t PageError = 0;
    FLASH_EraseInitTypeDef EraseInitStruct;
    EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.PageAddress = 0x0801FC00;
    EraseInitStruct.Banks = FLASH_BANK_1;
    EraseInitStruct.NbPages = 1;
    HAL_FLASHEx_Erase(&EraseInitStruct,&PageError) ;//如果结构体中的起始地址0x0801FC00，这一页的数据擦除成功，返回OK
    uint32_t writeFlashData = n;        //代写入的值
    uint32_t addr = 0x0801FC00;                  //写入的地址
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,addr, writeFlashData); //向FLASH中写入
    HAL_FLASH_Lock();
}
/**
  * @brief  读出存储地址中的内容
  * @param  用STM32中FLASH存储空间模拟EEPROM的读写
  * @retval 返回值：从FLASH中读出数据
  */
uint32_t FLASH_EEPROM_Read(void)
{
    HAL_FLASH_Unlock();
    uint32_t Page = 0;
    uint32_t addr = 0x0801FC00;                  //写入的地址
    Page=*(__IO uint32_t*)addr;
    return Page;
}
