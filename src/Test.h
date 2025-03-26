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

    void advanceCorrectionTest();

    void overspecificReliefTest();

    void turnLeftTest();

    void trackTurnLeftTest();
};


class K230Test {
public:
    void qrRecognizeTest();

    K230Test();
};

extern CarTest carTest;

extern K230Test k230Test;

#endif //ENTOURAGE_CLION_TEST_H
