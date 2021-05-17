#ifndef USRT_H
#define USRT_H

#include "stdio.h"
#include "usart.h"
#include "mode.h"
#include "upper.h"

extern uint8_t pc[2],EPC[12],RSSI,Upper_usart[256],ID_usart[256],Commannd[1],stop_flag;
extern uint8_t flag,dog,in,upper_flag,digit_now,empty,tid_step,write_step,TID[32],empty_flag;


#endif
