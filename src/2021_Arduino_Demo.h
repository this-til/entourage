//
// Created by til on 2025/2/26.
//

#ifndef ENTOURAGE_CLION_2021_ARDUINO_DEMO_H
#define ENTOURAGE_CLION_2021_ARDUINO_DEMO_H

#include "def.h"

void l_setup();

void l_loop();

void ZigBeeRx_Handler(uint8_t* mar);
// 处理ZigBee接收数据的函数，参数为指向uint8_t数组的指针

void OpenMVRx_Handler(uint8_t* mac);
// 处理OpenMV接收数据的函数，参数为指向uint8_t数组的指针

void Analyze_Handle(uint8_t com);
// 分析处理函数，参数为单字节命令

void KEY_Handler(uint8_t k_value);
// 处理按键的函数，参数为按键值

String DecIntToHexStr(long long num);
// 将十进制整数转换为十六进制字符串的函数，返回String类型

void OpenMVTrack_Disc_StartUp(void);
// 启动OpenMV追踪盘的函数

void OpenMVTrack_Disc_CloseUp(void);
// 关闭OpenMV追踪盘的函数

void Servoangle_control(uint8_t data);
// 舵机角度控制函数，参数为单字节数据

#endif //ENTOURAGE_CLION_2021_ARDUINO_DEMO_H

