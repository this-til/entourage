//
// Created by til on 2025/3/1.
//

#ifndef ENTOURAGE_CLION_UTIL_H
#define ENTOURAGE_CLION_UTIL_H

#include "def.h"

void logHex(uint8_t p);

void logHex(const uint8_t* p, uint16_t len);


uint32_t countBits(const uint8_t* value, uint32_t len);

uint32_t countBits(uint8_t uint8);

/***
 * 获取某位的bit
 * @param value
 * @param len
 * @param bit 第几位 [0,len*8)
 * @param fromLeftToRight 从左到右还是从右到左
 * @return
 */
uint8_t getBit(uint8_t* value, uint32_t len, int32_t bit, bool fromLeftToRight);

void setBit(uint8_t* value, uint32_t len, int32_t bit, bool set, bool fromLeftToRight);

/***
 * 排除一连串bit中的孤值
 *
 * 任何被0包围的1都将变成0
 *
 * 游戏检测 任何被0包围的1都将变成0
 *  例如 010100
 *  输出 000000
 *
 *  例如 011010
 *  输出 011000
 *
 * bit长度为8*len
 *
 *
 * @param value
 * @param len
 * @param outValue
 */
void lonelinessExclusion(uint8_t* value, uint32_t len, uint8_t* outValue);

/***
 * 计算中心分布
 * 输入 00011000
 * 输出 4
 *
 * 输入 00001000
 * 输出 4.5
 *
 * 输入 00000000
 * 输出 -1
 * @param value
 * @param len
 * @param centerShift 中心偏移
 * @return centralPoint
 */
float centralPoint(uint8_t* value, uint32_t len, float* centerShift);


#endif //ENTOURAGE_CLION_UTIL_H
