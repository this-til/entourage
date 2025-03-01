//
// Created by til on 2025/3/1.
//

#include "Util.h"

const char hex_chars[] = "0123456789ABCDEF";

void logHex(uint8_t p) {
#if DE_BUD
    Serial.print(hex_chars[(p >> 4) & 0x0F]);
    Serial.print(hex_chars[p & 0x0F]);
#endif
}

void logHex(const uint8_t* p, uint16_t len) {
#if DE_BUD
    for (int i = 0; i < len; ++i) {
        logHex(p[i]);
    }
#endif
}

