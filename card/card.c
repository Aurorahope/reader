//
// Created by lenovo on 2020/11/27.
//

#include "card.h"

#define GET_LOW_BYTE(A) ((uint8_t)(A))
#define GET_HIGH_BYTE(A) ((uint8_t)((A) >> 8))

uint8_t CheckSum(uint8_t *data, uint8_t len) {
    uint8_t temp = 0;

    data += 1;
    for (int i = 0; i < len; ++i) {
        temp += *data;
        data += 1;
    }

    return temp;
}

void SerialWrite(uint8_t *data, uint8_t len) {
    while (len--) {
        while ((CARD_USART->SR & 0X40U) == 0);
        CARD_USART->DR = *data++;
    }
}

void ID_Read(void) {
    uint8_t temp[10];

    temp[0] = 0xBB;
    temp[1] = 0x00;
    temp[2] = 0x27;
    temp[3] = 0x00;
    temp[4] = 0x03;
    temp[5] = 0x22;
    temp[6] = 0x27;
    temp[7] = 0x10;
    temp[8] = 0x83;
    temp[9] = 0x7E;

    SerialWrite(temp, 10);
}

void ID_STOP(void) {
    uint8_t temp[7];

    temp[0] = 0xBB;
    temp[1] = 0x00;
    temp[2] = 0x28;
    temp[3] = 0x00;
    temp[4] = 0x00;
    temp[5] = 0x28;
    temp[6] = 0x7E;

    SerialWrite(temp, 7);
}

void ID_BaudSet() {
    uint8_t temp[9];

    temp[0] = 0xBB;
    temp[1] = 0x00;
    temp[2] = 0x11;
    temp[3] = 0x00;
    temp[4] = 0x02;
    temp[5] = 0x04;
    temp[6] = 0x80;
    temp[7] = 0x97;
    temp[8] = 0x7E;

    SerialWrite(temp, 9);
}

void ID_AddressSet(void) {
    uint8_t temp[8];

    temp[0] = 0xBB;
    temp[1] = 0x00;
    temp[2] = 0x07;
    temp[3] = 0x00;
    temp[4] = 0x01;
    temp[5] = 0x01;
    temp[6] = 0x09;
    temp[7] = 0x7E;

    SerialWrite(temp, 8);
}

void ID_FreqSet(void) {
    uint8_t temp[8];

    temp[0] = 0xBB;
    temp[1] = 0x00;
    temp[2] = 0xAB;
    temp[3] = 0x00;
    temp[4] = 0x01;
    temp[5] = 0x01;
    temp[6] = 0xAD;
    temp[7] = 0x7E;

    SerialWrite(temp, 8);
}

void ID_PowerSet(uint8_t power_mode) {
    uint8_t temp[9];
    uint16_t power;
    uint8_t check;
    if (power_mode == 8) {
        power = 0x07D0;
        check = 0x8F;
    }
    if (power_mode == 7) {
        power = 0x073A;
        check = 0xF9;
    }
    if (power_mode == 6) {
        power = 0x06A4;
        check = 0x62;
    }
    if (power_mode == 5) {
        power = 0x060E;
        check = 0xCC;
    }
    if (power_mode == 4) {
        power = 0x0578;
        check = 0x35;
    }
    if (power_mode <= 3) {
        power = 0x04E2;
        check = 0x9E;
    }

    temp[0] = 0xBB;
    temp[1] = 0x00;
    temp[2] = 0xB6;
    temp[3] = 0x00;
    temp[4] = 0x02;
    temp[5] = GET_HIGH_BYTE(power);
    temp[6] = GET_LOW_BYTE(power);
    temp[7] = check;
    temp[8] = 0x7E;

    SerialWrite(temp, 9);
}

void Card_Init() {
    ID_BaudSet();
    ID_AddressSet();
    ID_FreqSet();
    ID_PowerSet(mode);
}

void TID_read1() {
    uint8_t temp[26], i;
    tid_step = 0;
    temp[0] = 0xBB;
    temp[1] = 0x00;
    temp[2] = 0x0C;
    temp[3] = 0x00;
    temp[4] = 0x13;
    temp[5] = 0x01;
    temp[6] = 0x00;
    temp[7] = 0x00;
    temp[8] = 0x00;
    temp[9] = 0x20;
    temp[10] = 0x60;
    temp[11] = 0x00;
    for (i = 0; i < 12; i++)
        temp[12 + i] = EPC[i];
    temp[24] = CheckSum(temp, 23);
    temp[25] = 0X7E;

    SerialWrite(temp, 26);
}

void TID_read2(uint8_t tid) {
    uint8_t temp[16];

    temp[0] = 0xBB;
    temp[1] = 0x00;
    temp[2] = 0x39;
    temp[3] = 0x00;
    temp[4] = 0x09;
    temp[5] = 0x00;
    temp[6] = 0x00;
    temp[7] = 0x00;
    temp[8] = 0x00;
    temp[9] = 0x02;
    temp[10] = 0x00;
    temp[11] = 0x00;
    temp[12] = 0x00;
    temp[13] = tid;
    temp[14] = CheckSum(temp, 13);
    temp[15] = 0x7E;

    SerialWrite(temp, 16);
}

void TID_write1() {
    uint8_t temp[26], i;
    tid_step = 0;
    temp[0] = 0xBB;
    temp[1] = 0x00;
    temp[2] = 0x0C;
    temp[3] = 0x00;
    temp[4] = 0x13;
    temp[5] = 0x21;
    temp[6] = 0x00;
    temp[7] = 0x00;
    temp[8] = 0x00;
    temp[9] = 0x20;
    temp[10] = 0x60;
    temp[11] = 0x00;
    for (i = 0; i < 12; i++)
        temp[12 + i] = EPC[i];
    temp[24] = CheckSum(temp, 23);
    temp[25] = 0X7E;

    SerialWrite(temp, 26);
}

void TID_write2() {
    uint8_t temp[16];

    temp[0] = 0xBB;
    temp[1] = 0x00;
    temp[2] = 0x39;
    temp[3] = 0x00;
    temp[4] = 0x09;
    temp[5] = 0x00;
    temp[6] = 0x00;
    temp[7] = 0x00;
    temp[8] = 0x00;
    temp[9] = 0x01;
    temp[10] = 0x00;
    temp[11] = 0x02;
    temp[12] = 0x00;
    temp[13] = 0x06;
    temp[14] = CheckSum(temp, 13);
    temp[15] = 0x7E;

    SerialWrite(temp, 16);

}

void TID_write3() {
    uint8_t temp[28],i;

    temp[0] = 0xBB;
    temp[1] = 0x00;
    temp[2] = 0x49;
    temp[3] = 0x00;
    temp[4] = 0x15;
    temp[5] = 0x00;
    temp[6] = 0x00;
    temp[7] = 0x00;
    temp[8] = 0x00;
    temp[9] = 0x01;
    temp[10] = 0x00;
    temp[11] = 0x02;
    temp[12] = 0x00;
    temp[13] = 0x06;
    for (i = 0; i < 12; i++)
        temp[14 + i] = epc_write[i];
    temp[26] = CheckSum(temp, 25);
    temp[27] = 0x7E;

    SerialWrite(temp, 28);

}
