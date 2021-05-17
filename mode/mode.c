//
// Created by lenovo on 2020/11/27.
//

#include "mode.h"

uint8_t mode = 0;

uint8_t mode_check(void) {
    if ((RSSI > 40+mode)&&mode!=30)
        return 0;
    else
        return 1;
}

