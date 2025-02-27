//
// Created by til on 2025/2/26.
//

#ifndef ENTOURAGE_CLION_DEF_H
#define ENTOURAGE_CLION_DEF_H

#include <DCMotor.h>
#include <CoreLED.h>
#include <CoreKEY.h>
#include <CoreBeep.h>
#include <ExtSRAMInterface.h>
#include <LED.h>
#include <BH1750.h>
#include <Command.h>
#include <BEEP.h>
#include <Infrare.h>
#include <BKRC_Voice.h>
#include <Arduino.h>
#include "Ultrasonic.h"

#include "UltrasonicUtil.h"
#include "2021_Arduino_Demo.h"
#include "CarControl.h"
#include "QrUtil.h"
#include "TrafficLightsUtil.h"
#include "Device.h"

// 标准无符号类型
#define uchar  unsigned char
#define ushort unsigned short
#define uint   unsigned int
#define ulong  unsigned long
#define ullong unsigned long long

// 标准有符号类型（明确长度）
#define int8  signed char
#define int16 signed short
#define int32 signed int
#define int64 signed long long

#define MAXIMUM_RETRY 50
#define WAITING_INTERVAL_MS 10

#define DATA_FRAME_HEADER 0x55
#define DATA_FRAME_TAIL 0xBB
#define BUS_BASE_ADD 0x6008

#endif //ENTOURAGE_CLION_DEF_H

