//
// Created by til on 2025/2/26.
//

#ifndef ENTOURAGE_CLION_2021_ARDUINO_DEMO_H
#define ENTOURAGE_CLION_2021_ARDUINO_DEMO_H

#include "def.h"

void l_setup();

void l_loop();

void KEY_Handler(uint8_t k_value);
// 处理按键的函数，参数为按键值

String DecIntToHexStr(long long num);
// 将十进制整数转换为十六进制字符串的函数，返回String类型

#endif //ENTOURAGE_CLION_2021_ARDUINO_DEMO_H

