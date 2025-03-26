//
// Created by til on 2025/3/13.
//

#ifndef ENTOURAGE_CLION_CACHE_H
#define ENTOURAGE_CLION_CACHE_H

#include "Device.h"

#define QR_CACHE_LEN 3
#define QR_CACHE_DATA_LEN 24

extern uint8_t message[QR_CACHE_LEN][QR_CACHE_DATA_LEN + 1];
extern struct QrMessage qrMessageArray[QR_CACHE_LEN];
extern uint8_t qrRecognizeBuf[32];

#define UNIVERSAL_CACHE_LEN 8
#define UNIVERSAL_CACHE_DATA_LEN 32

extern uint8_t universalCache[UNIVERSAL_CACHE_LEN][UNIVERSAL_CACHE_DATA_LEN + 1];

extern uint8_t preferredMatch[256];

#define DEBRIS_SPACE_ARRAY_LEN 8
#define DEBRIS_SPACE_LEN 16
#define DEBRIS_SPACE_MESSAGE_LEN 16

extern uint8_t debrisSpace[DEBRIS_SPACE_ARRAY_LEN][DEBRIS_SPACE_LEN][DEBRIS_SPACE_MESSAGE_LEN + 1];

extern const uint8_t bitCounts[256];
extern const int highestBitTable[256];
extern const int lowestBitTable[256];

void clearPreferredMatch();


void dataSetUp();


#endif //ENTOURAGE_CLION_CACHE_H
