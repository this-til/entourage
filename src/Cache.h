//
// Created by til on 2025/3/13.
//

#ifndef ENTOURAGE_CLION_CACHE_H
#define ENTOURAGE_CLION_CACHE_H

#include "Device.h"


struct Str {
    uint8_t* str;
    uint16_t len;
    uint16_t maxLen;
};

struct StrArray {
    Str* array;
    uint16_t len;
    uint16_t maxLen;
};

struct StrSlice {
    uint8_t* array;
    uint16_t len;
};

struct StrSliceArray {
    StrSlice* array;
    uint16_t len;
    uint16_t maxLen;
};

struct Point {
    uint16_t x;
    uint16_t y;
};

struct QrMessage {

    struct Str str;

    bool efficient = false;

    K210Color qrColor = K_WHITE;
};


#define QR_CACHE_LEN 4
#define QR_CACHE_DATA_LEN 24

extern QrMessage qrMessageArray[QR_CACHE_LEN];
extern uint8_t qrRecognizeBuf[32];

#define UNIVERSAL_CACHE_LEN 8
#define UNIVERSAL_CACHE_DATA_LEN 32

extern Str universalCache[UNIVERSAL_CACHE_LEN];

extern bool preferredMatch[256];

#define DEBRIS_SPACE_ARRAY_LEN 8
#define DEBRIS_SPACE_LEN 16
#define DEBRIS_SPACE_MESSAGE_LEN 16

//extern StrArray debrisSpace[DEBRIS_SPACE_ARRAY_LEN];

#define STR_SLICE_ARRAY_LEN 4
#define STR_SLICE_LEN 8

extern StrSliceArray strSliceArray[STR_SLICE_ARRAY_LEN];

extern const uint8_t hex_chars[];

extern const uint8_t bitCounts[256];
extern const int8_t highestBitTable[256];
extern const int8_t lowestBitTable[256];

void clearPreferredMatch();


void dataSetUp();


#endif //ENTOURAGE_CLION_CACHE_H
