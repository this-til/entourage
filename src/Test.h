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

    void reverseIntoTheCarportTest();

    void trimCarByLineTest();
};


class K210Test {
public:
    void qrRecognizeTest();

    K210Test();
};

extern CarTest carTest;

extern K210Test k210Test;

#endif //ENTOURAGE_CLION_TEST_H
