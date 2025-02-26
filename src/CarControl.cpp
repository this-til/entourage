//
// Created by til on 2025/2/26.
//

#include "CarControl.h"


void car_setCameraSteeringGearAngle(int8_t angle) {
    uint8_t servo_buf[8] = {0x55, 0x02, 0x91, 0x03, 0x00, 0x00, 0x00, 0xBB};

    if (angle >= 0) {
        servo_buf[4] = 0x2B;
    } else {
        servo_buf[4] = 0x2D;
    }

    servo_buf[5] = abs(angle);
    Command.Judgment(servo_buf);
    ExtSRAMInterface.ExMem_Write_Bytes(0x6008, servo_buf, 8);
    delay(1000);
}

/***
 * led闪烁
 */
void car_ledShine(int number, int wait) {
    for (int i = 0; i < number; ++i) {
        LED.LeftTurnOn();
        LED.RightTurnOn();
        delay(wait);
        LED.LeftTurnOff();
        LED.RightTurnOff();
    }
}