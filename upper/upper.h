//
// Created by lenovo on 2020/12/8.
//

#ifndef READER_UPPER_H
#define READER_UPPER_H

#define PRESET_VALUE 0xFFFF
#define POLYNOMIAL  0x8408

#include "stm32f1xx_hal.h"
#include "usrt.h"
#include "card.h"
#include "flash.h"

#define UPPER_USART USART1
#define GET_LOW_BYTE(A) ((uint8_t)(A))
#define GET_HIGH_BYTE(A) ((uint8_t)((A) >> 8))

extern uint16_t tid_flag,write_flag,epc_write[12],mode_flag;

void UpperWrite(uint8_t *data, uint8_t len) ;
unsigned int uiCrc16Cal(unsigned char const  * pucY, unsigned char ucX) ;
void upper_back(uint8_t command) ;
void upper_back_w() ;
void upper_get();
uint8_t CheckSum_1(uint8_t *data, uint8_t len) ;

#endif //READER_UPPER_H
