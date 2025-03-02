//
// Created by til on 2025/3/2.
//

#include "Math.h"

uint16_t countBits(const uint8_t* value, uint16_t len) {
    uint16_t count = 0;
    for (int i = 0; i < len; ++i) {
        for (uint8_t ii = 0; ii < 8; ii++) {
            if (value[i] & (1 << ii)) {
                count++;
            }
        }
    }
    return count;
}

uint16_t countBits(uint8_t uint8) {
    return countBits(&uint8, 1);
}

void lonelinessExclusion(const bool* value, uint32_t len, bool* outValue) {

    for (uint32_t i = 0; i < len; ++i) {
        outValue[i] = value[i];
    }

    for (uint32_t i = 0; i < len; ++i) {

    }
}

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
 *
 *
 * @param value
 * @param len
 * @param outValue
 */
void lonelinessExclusion(const uint8_t* value, uint32_t len, uint8_t* outValue) {
    uint32_t mLen = len * 8;

    for (uint32_t i = 0; i < len; ++i) {
        outValue[i] = value[i];
    }


    for (int32_t i = 0; i < mLen; ++i) {
        int32_t beforeIn = i - 1;
        int32_t afterIn = i + 1;

        uint8_t before = beforeIn < 0
                         ? 0
                         : (value[beforeIn / 8] >> (7 - (beforeIn % 8))) & 0x01;
        //bool current = (value[i / 8] >> (i % 8)) & 0x01;
        uint8_t after = afterIn >= mLen
                        ? 0
                        : (value[afterIn / 8] >> (7 - (afterIn % 8))) & 0x01;

        if (!before && !after) {
            outValue[i / 8] &= ~(0x80 >> (i % 8));
        }

    }
}