//
// Created by til on 2025/3/13.
//

#ifndef ENTOURAGE_CLION_CACHE_H
#define ENTOURAGE_CLION_CACHE_H

#include "Device.h"

extern uint8_t message[4][24];
extern struct QrMessage qrMessageArray[4];
extern uint8_t qrRecognizeBuf[64];

void dataSetUp();


#endif //ENTOURAGE_CLION_CACHE_H
