//
// Created by til on 2025/3/13.
//

#include "Cache.h"

uint8_t message[4][24] = {};
QrMessage qrMessageArray[4] = {};
uint8_t qrRecognizeBuf[64] = {};

void dataSetUp() {
    for (int i = 0; i < 4; ++i) {
        qrMessageArray[i].messageMaxLen = 24;
        qrMessageArray[i].message = message[i];
    }
}
