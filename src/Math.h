//
// Created by til on 2025/3/2.
//

#ifndef ENTOURAGE_CLION_MATH_H
#define ENTOURAGE_CLION_MATH_H

#include <stdint-gcc.h>
#include "def.h"

uint16_t countBits(const uint8_t* value, uint16_t len);

uint16_t countBits(uint8_t uint8);

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
void lonelinessExclusion(const bool* value, uint32_t len, bool* outValue);

/***
 * 计算bit的中心分布
 *  输入:00011000
 *  输出:4
 *
 *  输入:00010000
 *  输出:3.5
 *
 *  输入:0000000110000000
 *  输出:8
 *
 * @param value
 * @param len
 * @param lonelinessExclusion
 *  输入 01011000
 *  输出:4
 *  因为 010, 1的数量  <= lonelinessExclusion,而两边0的数量 >= lonelinessExclusion ，所以将排除
 *
 *  输入 00101100
 *  输出:4
 *  因为 101 0数量 <= lonelinessExclusion ,而1数量>=lonelinessExclusion  所以将0视为1
 * @return
 */
float centralPoint(const uint8_t* value, uint16_t len, uint16_t lonelinessExclusion = 1);

#endif //ENTOURAGE_CLION_MATH_H
