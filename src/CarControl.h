//
// Created by til on 2025/2/26.
//

#ifndef ENTOURAGE_CLION_CARCONTROL_H
#define ENTOURAGE_CLION_CARCONTROL_H

#include <def.h>

/***
 * 设置摄像头舵机角度
 * @param angle 角度 舵机角度，范围-80至+40，0度垂直于车身
 */
void car_setCameraSteeringGearAngle(int8_t angle);

/***
 * led闪烁
 * @param number 次数
 * @param wait 闪烁间隔
 */
void car_ledShine(int number, int wait);


#endif //ENTOURAGE_CLION_CARCONTROL_H
