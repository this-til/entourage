//
// Created by til on 2025/2/26.
//


#define DE_BUG false

#define FLASHEND 0x3FFFF

/*
#define LOG_LEVEL 4  // 0=OFF,1=ERROR,2=WARN,3=INFO,4=DEBUG

#define LOG_FATAL  (LOG_LEVEL >= 0)
#define LOG_ERROR  (LOG_LEVEL >= 1)
#define LOG_WARN   (LOG_LEVEL >= 2)
#define LOG_INFO   (LOG_LEVEL >= 3)
#define LOG_DEBUG  (LOG_LEVEL >= 4)
*/

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
#include "SCoop.h"
#include "WString.h"

#include "2021_Arduino_Demo.h"
#include "Device.h"
#include "Util.h"
#include "Enum.h"
#include "Test.h"
#include "Cache.h"
#include "Questions.h"

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
#define BUD_K210_RECEIVE 0x6038

#endif //ENTOURAGE_CLION_DEF_H

