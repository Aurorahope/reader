//
// Created by lenovo on 2020/8/28.
//

#include "usrt.h"

uint8_t ID_usart[256] = {0}, IDcard_usart[256] = {0}, Upper_usart[256], Commannd[1] = {0}, pc[2], upper[256], TID[32];
uint8_t EPC[12], RSSI = 0, empty, num_h, num_l, flag_upper, stop_flag, write_step, empty_flag, err;
uint8_t flag = 0, i, digit_now = 0, digit_sum = 0, upper_flag = 0, tid_step;

uint8_t EPC_sum_check(void) {
    uint32_t EPC_sum;

    EPC_sum = 0;
    for (i = 0; i < 12; i++) {
        EPC_sum = EPC_sum + IDcard_usart[i];
    }
    if (EPC_sum == 0)
        return 0;
    else
        return 1;
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == huart1.Instance) {
        if (flag_upper == 1) {
            HAL_UART_Receive_IT(&huart1, Commannd, 1);
            if (upper_flag == 0) {
                for (i = 0; i < digit_sum; i++)
                    Upper_usart[i + 1] = upper[i];
                upper_flag = 1;
            }
            flag_upper = 0;
        } else if ((Commannd[0] <= 0x09 && Commannd[0] > 3) || Commannd[0] == 0x18 || Commannd[0] == 0x15) {
            if (upper_flag == 0)
                Upper_usart[0] = Commannd[0];
            digit_sum = Commannd[0];
            HAL_UART_Receive_IT(&huart1, upper, digit_sum);
            flag_upper = 1;
        } else {
            HAL_UART_Receive_IT(&huart1, Commannd, 1);
        }
    }
    if (huart->Instance == huart2.Instance) {
        if (ID_usart[0] == 0x7e) {
            HAL_UART_Receive_IT(&huart2, ID_usart, 8);
            err = 0;
        } else if (err == 1)
            HAL_UART_Receive_IT(&huart2, ID_usart, 1);
        else if (ID_usart[2] == 0x28) {
            stop_flag = 1;
            if (ID_usart[7] != 0x7e) {
                err = 1;
                HAL_UART_Receive_IT(&huart2, ID_usart, 1);
            }
            HAL_UART_Receive_IT(&huart2, ID_usart, 8);
        } else if (tid_step == 2) {
            if (IDcard_usart[25] != 0x7e) {
                err = 1;
                HAL_UART_Receive_IT(&huart2, ID_usart, 1);
            }
            for (i = 0; i < 12; i++)
                TID[i] = IDcard_usart[i + 12];
            tid_step = 3;
            HAL_UART_Receive_IT(&huart2, ID_usart, 8);
        } else if (write_step == 2) {
            write_step = 3;
            if (IDcard_usart[25] != 0x7e) {
                err = 1;
                HAL_UART_Receive_IT(&huart2, ID_usart, 1);
            }
            HAL_UART_Receive_IT(&huart2, ID_usart, 8);
        } else if (write_step == 4) {
            write_step = 5;
            if (IDcard_usart[14] != 0x7e) {
                err = 1;
                HAL_UART_Receive_IT(&huart2, ID_usart, 1);
            }
            HAL_UART_Receive_IT(&huart2, ID_usart, 8);
        } else if (ID_usart[2] == 0x49) {
            write_step = 4;
            HAL_UART_Receive_IT(&huart2, IDcard_usart, 15);
        } else if (ID_usart[2] == 0x0c) {
            if (tid_flag == 1) {
                tid_step = 1;
                if (ID_usart[7] != 0x7e) {
                    err = 1;
                    HAL_UART_Receive_IT(&huart2, ID_usart, 1);
                }
                HAL_UART_Receive_IT(&huart2, ID_usart, 8);
            }
            if (write_flag == 1) {
                write_step = 1;
                if (ID_usart[7] != 0x7e) {
                    err = 1;
                    HAL_UART_Receive_IT(&huart2, ID_usart, 1);
                }
                HAL_UART_Receive_IT(&huart2, ID_usart, 8);
            }
        } else if (ID_usart[2] == 0x39) {
            if (tid_flag) {
                HAL_UART_Receive_IT(&huart2, IDcard_usart, 26);
                tid_step = 2;
            } else if (write_flag) {
                write_step = 2;
                HAL_UART_Receive_IT(&huart2, IDcard_usart, 26);
            }
        } else if (flag == 1) {
            if (IDcard_usart[15] != 0x7e) {
                err = 1;
                HAL_UART_Receive_IT(&huart2, ID_usart, 1);
            }
            flag = 0;
            if (EPC_sum_check() && mode_check()) {
                for (i = 0; i < 12; i++)
                    EPC[i] = IDcard_usart[i];
                empty = 1;
                empty_flag = 1;
            }
            HAL_UART_Receive_IT(&huart2, ID_usart, 8);
        } else if (ID_usart[2] == 0x22) {
            HAL_UART_Receive_IT(&huart2, IDcard_usart, 16);
            RSSI = 255 - ID_usart[5];
            flag = 1;
        } else if (ID_usart[2] == 0XFF) {
            if (ID_usart[7] != 0x7e) {
                err = 1;
                HAL_UART_Receive_IT(&huart2, ID_usart, 1);
            }
            HAL_UART_Receive_IT(&huart2, ID_usart, 8);
        }
    }
}

