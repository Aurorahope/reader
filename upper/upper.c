//
// Created by lenovo on 2020/12/8.
//

#include "upper.h"

#define address_upper 0x00
#define PRESET_VALUE 0xFFFF
#define POLYNOMIAL  0x8408

uint16_t write, cRc, tid_flag, write_flag, epc_write[12], epc_tid[24], same, tid_get,mode_flag;

uint8_t CheckSum_1(uint8_t *data, uint8_t len) {
    uint8_t temp = 0;

    for (int i = 0; i < len - 1; i++) {
        temp += *data++;
    }

    return temp;
}


unsigned int uiCrc16Cal(unsigned char const *pucY, unsigned char ucX) {
    unsigned char ucI, ucJ;
    unsigned short int uiCrcValue = PRESET_VALUE;

    for (ucI = 0; ucI < ucX; ucI++) {
        uiCrcValue = uiCrcValue ^ *(pucY + ucI);
        for (ucJ = 0; ucJ < 8; ucJ++) {
            if (uiCrcValue & 0x0001) {
                uiCrcValue = (uiCrcValue >> 1) ^ POLYNOMIAL;
            } else {
                uiCrcValue = (uiCrcValue >> 1);
            }
        }
    }
    return uiCrcValue;
}

void UpperWrite(uint8_t *data, uint8_t len) {
    while (len--) {
        while ((UPPER_USART->SR & 0X40U) == 0);
        UPPER_USART->DR = *data++;
    }
}

void upper_get() {
    upper_flag = 0;
    cRc = uiCrc16Cal(Upper_usart, Upper_usart[0] - 1);
    if ((address_upper == Upper_usart[1] || Upper_usart[1] == 0xff)
        && (GET_LOW_BYTE(cRc) == Upper_usart[Upper_usart[0] - 1])
        && (GET_HIGH_BYTE(cRc) == Upper_usart[Upper_usart[0]])
            )
        upper_back(Upper_usart[2]);
    else
        upper_back(0);

}

void upper_back_w() {
    uint8_t upper[128],i,digit_back;
    if (empty == 1) {
        digit_back=0X10;
        upper[0] = 0XAA;
        upper[1] = digit_back-2;
        upper[2] = 0X01;
        for (i = 0; i < 12; i++)
            upper[3 + i] = EPC[i];
        upper[15] = CheckSum_1(upper,digit_back);
        empty = 0;
    } else {
        digit_back = 0X04;
        upper[0] = 0XAA;
        upper[1] = digit_back-2;
        upper[2] = 0X01;
        upper[3] = CheckSum_1(upper,digit_back);
    }
    UpperWrite(upper, digit_back);
}


void upper_back(uint8_t command) {
    uint8_t upper[128], j;
    uint8_t digit_back, reCmd, status;
    uint32_t i;
    switch (command) {
        case 0:
            digit_back = 6;
            reCmd = 0X00;
            status = 0XFE;
            upper[0] = digit_back - 1;
            upper[1] = address_upper;
            upper[2] = reCmd;
            upper[3] = status;
            cRc = uiCrc16Cal(upper, digit_back - 2);
            upper[4] = GET_LOW_BYTE(cRc);
            upper[5] = GET_HIGH_BYTE(cRc);
            break;
        case 1:
            i = 0;
            while (empty == 0 && i < 1000)
                i++;
            if (empty == 1) {
                digit_back = 20;
                reCmd = 0x01;
                status = 0x01;
                upper[0] = digit_back - 1;
                upper[1] = address_upper;
                upper[2] = reCmd;
                upper[3] = status;
                upper[4] = 0x01;
                upper[5] = 0X0C;
                for (i = 0; i < 12; i++)
                    upper[6 + i] = EPC[i];
                cRc = uiCrc16Cal(upper, digit_back - 2);
                upper[18] = GET_LOW_BYTE(cRc);
                upper[19] = GET_HIGH_BYTE(cRc);
                empty = 0;
            } else {
                digit_back = 8;
                reCmd = 0x01;
                status = 0x02;
                upper[0] = digit_back - 1;
                upper[1] = address_upper;
                upper[2] = reCmd;
                upper[3] = status;
                upper[4] = 0x00;
                upper[5] = 0X00;
                cRc = uiCrc16Cal(upper, digit_back - 2);
                upper[6] = GET_LOW_BYTE(cRc);
                upper[7] = GET_HIGH_BYTE(cRc);
            }
            break;
        case 2:
            same = 1;
            for (i = 0; i < 12; i++) {
                if (epc_tid[i] != Upper_usart[4 + i])
                    same = 0;
            }
            for (i = 0; i < 12; i++) {
                epc_tid[i] = Upper_usart[4 + i];
            }
            if (same == 1 && tid_get == 1) {
                digit_back = 6 + Upper_usart[18] * 2;
                reCmd = 0X02;
                status = 0X00;
                upper[0] = digit_back - 1;
                upper[1] = address_upper;
                upper[2] = reCmd;
                upper[3] = status;
                for (i = 0; i < Upper_usart[18] * 2; i++) {
                    upper[4 + i] = epc_tid[12 + i];
                }
                cRc = uiCrc16Cal(upper, digit_back - 2);
                upper[4 + Upper_usart[18] * 2] = GET_LOW_BYTE(cRc);
                upper[5 + Upper_usart[18] * 2] = GET_HIGH_BYTE(cRc);
            } else {
                tid_get=0;
                tid_flag = 1;
                i = 0;
                j = 0;
                empty_flag = 1;
                while (empty == 0 && i < 1000)
                    i++;
                while (tid_step == 0 && j < 100) {
                    empty_flag = 1;
                    j++;
                    TID_read1();
                    i = 0;
                    while (tid_step == 0 && i < 65535)
                        i++;
                }
                j = 0;
                while (tid_step == 1 && j < 100) {
                    empty_flag = 1;
                    j++;
                    TID_read2(6);
                    i = 0;
                    while (tid_step == 1 && i < 65535)
                        i++;
                }
                i = 0;
                while (tid_step == 2 && i < 65535)
                    i++;
                if (tid_step >= 2) {
                    tid_get = 1;
                    empty_flag = 1;
                    digit_back = 6 + Upper_usart[18] * 2;
                    reCmd = 0X02;
                    status = 0X00;
                    upper[0] = digit_back - 1;
                    upper[1] = address_upper;
                    upper[2] = reCmd;
                    upper[3] = status;
                    for (i = 0; i < 12; i++) {
                        upper[4 + i] = TID[i];
                        epc_tid[i + 12] = TID[i];
                    }
                    cRc = uiCrc16Cal(upper, digit_back - 2);
                    upper[4 + Upper_usart[18] * 2] = GET_LOW_BYTE(cRc);
                    upper[5 + Upper_usart[18] * 2] = GET_HIGH_BYTE(cRc);
                } else {
                    tid_get = 0;
                    digit_back = 6;
                    reCmd = 0X00;
                    status = 0XFE;
                    upper[0] = digit_back - 1;
                    upper[1] = address_upper;
                    upper[2] = reCmd;
                    upper[3] = status;
                    cRc = uiCrc16Cal(upper, digit_back - 2);
                    upper[4] = GET_LOW_BYTE(cRc);
                    upper[5] = GET_HIGH_BYTE(cRc);
                }
//            i = 0;
//            HAL_GPIO_WritePin(module_reset_GPIO_Port, module_reset_Pin, GPIO_PIN_RESET);
//            empty_flag = 1;
//            HAL_UART_Receive_IT(&huart2, ID_usart, 8);
//            HAL_Delay(200);
//            HAL_GPIO_WritePin(module_reset_GPIO_Port, module_reset_Pin, GPIO_PIN_SET);
                tid_step = 0;
                tid_flag = 0;
            }
            break;
        case 3:
            digit_back = 6;
            reCmd = 0X03;
            status = 0X00;
            upper[0] = digit_back - 1;
            upper[1] = address_upper;
            upper[2] = reCmd;
            upper[3] = status;
            cRc = uiCrc16Cal(upper, digit_back - 2);
            upper[4] = GET_LOW_BYTE(cRc);
            upper[5] = GET_HIGH_BYTE(cRc);
            break;
        case 4:
            write = 1;
            for (i = 0; i < 12; i++) {
                epc_write[i] = Upper_usart[8 + i];
                if (epc_write[i] != EPC[i])
                    write = 0;
            }
            if (write == 1) {
                digit_back = 6;
                reCmd = 0X04;
                status = 0X00;
                upper[0] = digit_back - 1;
                upper[1] = address_upper;
                upper[2] = reCmd;
                upper[3] = status;
                cRc = uiCrc16Cal(upper, digit_back - 2);
                upper[4] = GET_LOW_BYTE(cRc);
                upper[5] = GET_HIGH_BYTE(cRc);
            } else {
                write_flag = 1;
                i = 0;
                while (empty == 0 && i < 1000)
                    i++;
                j = 0;
                while (write_step == 0 && j < 100) {
                    j++;
                    TID_write1();
                    i = 0;
                    while (write_step == 0 && i < 200000)
                        i++;
                }
                j = 0;
                while (write_step == 1 && j < 100) {
                    j++;
                    TID_write2();
                    i = 0;
                    while (write_step == 1 && i < 65535)
                        i++;
                }
                j = 0;
                i = 0;
                while (write_step == 2 && i < 65535)
                    i++;
                while (write_step == 3 && j < 100) {
                    j++;
                    TID_write3();
                    i = 0;
                    while (write_step == 3 && i < 65535)
                        i++;
                }
                i = 0;
                while (write_step == 4 && i < 65535)
                    i++;
                if (write_step >= 3) {
                    digit_back = 6;
                    reCmd = 0X04;
                    status = 0X00;
                    upper[0] = digit_back - 1;
                    upper[1] = address_upper;
                    upper[2] = reCmd;
                    upper[3] = status;
                    cRc = uiCrc16Cal(upper, digit_back - 2);
                    upper[4] = GET_LOW_BYTE(cRc);
                    upper[5] = GET_HIGH_BYTE(cRc);
                } else {
                    digit_back = 6;
                    reCmd = 0X00;
                    status = 0XFE;
                    upper[0] = digit_back - 1;
                    upper[1] = address_upper;
                    upper[2] = reCmd;
                    upper[3] = status;
                    cRc = uiCrc16Cal(upper, digit_back - 2);
                    upper[4] = GET_LOW_BYTE(cRc);
                    upper[5] = GET_HIGH_BYTE(cRc);
                }
            }
//            HAL_GPIO_WritePin(module_reset_GPIO_Port, module_reset_Pin, GPIO_PIN_RESET);
//            empty_flag = 1;
//            HAL_UART_Receive_IT(&huart2, ID_usart, 8);
//            HAL_Delay(200);
//            HAL_GPIO_WritePin(module_reset_GPIO_Port, module_reset_Pin, GPIO_PIN_SET);
            write_flag = 0;
            write_step = 0;
            break;
        case 0x21:
            digit_back = 0x0e;
            reCmd = 0x21;
            status = 0x00;
            upper[0] = digit_back - 1;
            upper[1] = address_upper;
            upper[2] = reCmd;
            upper[3] = status;
            upper[4] = 0x03;
            upper[5] = 0x3F;
            upper[6] = 0x09;
            upper[7] = 0x03;
            upper[8] = 0x3E;
            upper[9] = 0x00;
            upper[10] = mode;
            upper[11] = 0xff;
            cRc = uiCrc16Cal(upper, digit_back - 2);
            upper[12] = GET_LOW_BYTE(cRc);
            upper[13] = GET_HIGH_BYTE(cRc);
            break;
        case 0x22:
            digit_back = 0x06;
            reCmd = 0x22;
            status = 0x00;
            upper[0] = digit_back - 1;
            upper[1] = address_upper;
            upper[2] = reCmd;
            upper[3] = status;
            cRc = uiCrc16Cal(upper, digit_back - 2);
            upper[4] = GET_LOW_BYTE(cRc);
            upper[5] = GET_HIGH_BYTE(cRc);
            break;
        case 0x24:
            digit_back = 0x06;
            reCmd = 0x24;
            status = 0x00;
            upper[0] = digit_back - 1;
            upper[1] = address_upper;
            upper[2] = reCmd;
            upper[3] = status;
            cRc = uiCrc16Cal(upper, digit_back - 2);
            upper[4] = GET_LOW_BYTE(cRc);
            upper[5] = GET_HIGH_BYTE(cRc);
            break;
        case 0x25:
            digit_back = 0x06;
            reCmd = 0x25;
            status = 0x00;
            upper[0] = digit_back - 1;
            upper[1] = address_upper;
            upper[2] = reCmd;
            upper[3] = status;
            cRc = uiCrc16Cal(upper, digit_back - 2);
            upper[4] = GET_LOW_BYTE(cRc);
            upper[5] = GET_HIGH_BYTE(cRc);
            break;
        case 0x28:
            digit_back = 0x06;
            reCmd = 0x28;
            status = 0x00;
            upper[0] = digit_back - 1;
            upper[1] = address_upper;
            upper[2] = reCmd;
            upper[3] = status;
            cRc = uiCrc16Cal(upper, digit_back - 2);
            upper[4] = GET_LOW_BYTE(cRc);
            upper[5] = GET_HIGH_BYTE(cRc);
            break;
        case 0x2F:
            digit_back = 0x06;
            reCmd = 0x2F;
            status = 0x00;
            mode = Upper_usart[3];
            mode_flag=1;
            upper[0] = digit_back - 1;
            upper[1] = address_upper;
            upper[2] = reCmd;
            upper[3] = status;
            cRc = uiCrc16Cal(upper, digit_back - 2);
            upper[4] = GET_LOW_BYTE(cRc);
            upper[5] = GET_HIGH_BYTE(cRc);
            break;
        default:
            digit_back = 6;
            reCmd = 0X00;
            status = 0XFE;
            upper[0] = digit_back - 1;
            upper[1] = address_upper;
            upper[2] = reCmd;
            upper[3] = status;
            cRc = uiCrc16Cal(upper, digit_back - 2);
            upper[4] = GET_LOW_BYTE(cRc);
            upper[5] = GET_HIGH_BYTE(cRc);
            break;
    }
    UpperWrite(upper, digit_back);
}
