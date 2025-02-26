//
// Created by til on 2025/2/26.
//

#ifndef ENTOURAGE_CLION_ULTRASONICUTIL_H
#define ENTOURAGE_CLION_ULTRASONICUTIL_H


#include <def.h>


/***
 * 超声波测距
 * @return 测得距离
 */
double ultrasonic_ranging(int sys = CM, int rangeFrequency = 5, int wait = 30);

/***
 * 通过超声波测距控制测距
 * 该操作会暂停小车
 * @param carSeep 车速
 * @param distance 距离
 * @param errorMargin 允许的误差
 * @param controlTime_ms 调整时间，如果超过时间无论有没有到指定距离都将退出函数
 * @return 如果true表示调整到指定距离
 */
void ultrasonic_adjustDistance(uint8_t carSeep, uint8_t targetDistance, double errorMargin = 0.2, ulong controlTime_ms = 2000, int wait = 30);

#endif //ENTOURAGE_CLION_ULTRASONICUTIL_H
