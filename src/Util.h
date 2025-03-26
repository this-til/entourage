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
 * 求value中1的数量
 * @param value
 * @param len
 * @param starting  ~ [0, len * 8) 开始的bit索引位置
 * @param end       ~ (0, len * 8] 结束的bit索引+1的位置
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
void excludeSpecialCharacter(const uint8_t* source, uint16_t sourceLen, uint8_t* out, uint16_t outMaxLen, uint16_t* outLen);

/***
 * 通过matchingItem排除特定字符
 * @param matchingItem len = 256 Map<uint8_t, bool> if v true 将排除
 */
void excludeCharacter(const uint8_t* source, uint16_t sourceLen, uint8_t* out, uint16_t outMaxLen, uint16_t* outLen, uint8_t* matchingItem);

/***
 * 提取指定up、down中间或者外面的数据
 */
void extractCharacter(const uint8_t* str, uint16_t strLen, uint8_t* outStr, uint16_t maxLen, uint16_t* outLen, uint8_t up = '<', uint8_t down = '>', bool inRadius = true);

/***
 * 深度提取字符
 * 
 */
void profundityExtractCharacter(const uint8_t* str, uint16_t strLen, uint8_t* outStr, uint16_t maxLen, uint16_t* outLen, uint8_t up = '<', uint8_t down = '>', bool inRadius = true);

/***
 * 匹配特定字符
 * @param str 源
 * @param strLen 源长度
 * @param matchingEntry 匹配项
 * @param matchingEntryLen 匹配项长度
 * @param preferredMatch 第一个匹配成功的字符索引
 * @return
 */
bool inclusiveCharacter(const uint8_t* str, uint16_t strLen, uint8_t* matchingEntry, uint16_t matchingEntryLen, uint16_t* preferredMatch = nullptr);

/***
 * 找不同
 * 2023国赛二维码字符处理
 * 二维码（1）信息 A1B2C3D4E5F6
 * 二维码（2）信息 A1B2D1D4E5F7
 * 按位比对 2 个二维码中的有效数据，提取出不同数据，以二维码（1）中与二
 * 维码（2）中不同数据在前，二维码（2）与二维码（1）中不同数据在后为原则，
 * 得到 C，3，6，D，1，7 数据
 *
 * @param out len为偶数，不然将会舍弃最后一位
 */
void spotTheDifference(const uint8_t* strA, const uint8_t* strB, uint16_t strLen, uint8_t* out, uint16_t maxOutLen, uint16_t* countOfDifferentTerms = nullptr);

/***
 * 将两位ascii字符(0~F)组成一位16进制的字符
 */
void asciiToHex(const uint8_t* str, uint16_t strLen, uint8_t* out, uint16_t maxOutLen, uint16_t* outLen);


/***
 * 表达式求值
 * @param expression 例如 ((n*y+h)^4)/100
 * @param variable 长度为256的数组 使用variable['n']获取变量
 * @return
 */
int16_t

evaluateTheExpression(const uint8_t* expression, uint16_t variable[]);

#endif //ENTOURAGE_CLION_UTIL_H
