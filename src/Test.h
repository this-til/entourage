//
// Created by til on 2025/3/11.
//

#ifndef ENTOURAGE_CLION_TEST_H
#define ENTOURAGE_CLION_TEST_H

#include "def.h"

class CarTest {
public:

    void trackAdvanceToNextJunctionTest();

/***
 * 让车进行八字循环
 */
    void figureEightCycle();

    void mobileCorrectionTest();

    void rightCarTest();
};

extern CarTest carTest;

#endif //ENTOURAGE_CLION_TEST_H
