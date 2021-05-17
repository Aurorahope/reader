//
// Created by lenovo on 2020/11/27.
//

#ifndef READER_CARD_H
#define READER_CARD_H

#include "stm32f1xx_hal.h"
#include "usart.h"
#include "usrt.h"

#define CARD_USART USART2

uint8_t CheckSum(uint8_t *data, uint8_t len) ;
void CardWrite(uint8_t *data, uint8_t len) ;
void CardRead(void) ;
void ID_Read(void) ;
void ID_STOP(void) ;
void ID_AddressSet(void) ;
void ID_FreqSet(void) ;
void ID_PowerSet(uint8_t power_mode) ;
void ID_BaudSet(void) ;
void Card_Init();
void TID_read1();
void TID_read2(uint8_t tid);
void TID_write1();
void TID_write2();
void TID_write3();


#endif //READER_CARD_H
