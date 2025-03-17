//
// Created by til on 2025/3/1.
//


#ifndef ENTOURAGE_CLION_UTIL_H
#define ENTOURAGE_CLION_UTIL_H

#include "def.h"

#define tristate(v, min, z, max) v > 0 ? max : v < 0 ? min : z
#define inRand(v, min, max) v >= min && v <= max
#define clamp(v, min, max) v < min ? min : v > max ? max : v

void logHex(uint8_t p);

void logHex(uint16_t p);

void logHex(const uint8_t* p, uint16_t len);

void logHex(const uint16_t* p, uint16_t len);

void logBool(bool b);

void logBin(uint8_t data);

bool equals(uint16_t* a, uint16_t* b, uint8_t len);

uint32_t countBits(const uint8_t* value, uint32_t len);

/***
 *
 * @param value
 * @param len
 * @param starting  ~ [starting, end)
 * @param end       ~
 * @return
 */
uint32_t countBits(const uint8_t* value, uint32_t len, uint32_t starting, uint32_t end);

uint32_t countBits(uint8_t uint8);


/***
 * 获取某位的bit
 * @param value
 * @param len
 * @param bit 第几位 [0,len*8)
 * @param fromLeftToRight 从左到右还是从右到左
 * @return
 */
uint8_t getBit(const uint8_t* value, uint32_t len, int32_t bit, bool fromLeftToRight);

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
 * 输入 00001110
 * 输出 5.5
 *
 * 输入 00000000
 * 输出 -1
 * @param value
 * @param len
 * @param centerShift 中心偏移
 * @param centerShift 中心偏移[-1,1]
 * @return centralPoint
 */
float centralPoint(uint8_t* value, uint32_t len, float* centerShift, float* centerShiftOne);

struct Pos {
    uint8_t x;
    uint8_t y;
};

Pos pack(uint16_t v);

uint16_t unpack(Pos pos);

/***
 * 将两位字符串组合成uint16_t
 * "F1" -> 'F' << 8 | '1'
 * @return
 */
uint16_t assembly(const char* str);

void analyze(uint16_t* array, uint8_t maxArrayLen, char* outStr, uint8_t maxStrLen);

void assembly(const char* str, uint16_t* outArray, uint8_t maxArrayLen);

/***
 * 排除source中的特殊字符，并且输出到out
 */
void excludeSpecialCharacter(const uint8_t* source, uint8_t sourceLen, uint8_t* out, uint8_t outLen, uint8_t* specialNumber = nullptr);

/***
 * 表达式求值
 * @param expression 例如 ((n*y+h)^4)/100
 * @param variable 长度为256的数组 使用variable['n']获取变量
 * @return
 */
int16_t evaluateTheExpression(const uint8_t* expression, uint16_t variable[]);

#endif //ENTOURAGE_CLION_UTIL_H
