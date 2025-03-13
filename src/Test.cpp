//
// Created by til on 2025/3/11.
//

#include "Test.h"


CarTest carTest;
K230Test k230Test;

/*void trimCarTest() {

#if DE_BUG
    Serial.println("[DEBUG] in trackAdvanceToNextJunction...");
#endif

    car.trimCar();


#if DE_BUG
    Serial.println("[DEBUG] end trackAdvanceToNextJunction...");
#endif
}*/

void CarTest::figureEightCycle() {
    k230.setCameraSteeringGearAngle(-80);
    k230.setTrackModel(TRACK_MIDDLE);
    for (int i = 0; i < 100; ++i) {
        car.trackAdvanceToNextJunction();

        car.turnLeft(false);
        car.recoil(100);
        car.trimCar();
        car.recoilToNextJunction();
        car.advance(50);
        car.overspecificRelief();


        car.trackAdvanceToNextJunction();
        car.trackTurnLeft();
        car.trackAdvanceToNextJunction();
        car.trackTurnLeft();
        car.trackAdvanceToNextJunction();
        car.trackTurnLeft();

        car.trackAdvanceToNextJunction();
        car.trackTurnRight();
        car.trackAdvanceToNextJunction();
        car.trackTurnRight();
        car.trackAdvanceToNextJunction();
        car.trackTurnRight();
        car.trackAdvanceToNextJunction();
        car.trackTurnRight();
    }
    k230.setTrackModel(0);
}


void CarTest::trackAdvanceToNextJunctionTest() {
    k230.setCameraSteeringGearAngle(-80);
    k230.setTrackModel(TRACK_MIDDLE);
    car.trackAdvanceToNextJunction();
    k230.setTrackModel(false);
}

void CarTest::mobileCorrectionTest() {
    k230.setCameraSteeringGearAngle(-80);
    k230.setTrackModel(TRACK_MIDDLE);
    car.mobileCorrection(200);
    k230.setTrackModel(false);
}

void CarTest::rightCarTest() {
    k230.setCameraSteeringGearAngle(-80);
    k230.setTrackModel(TRACK_MIDDLE);
    car.rightCar();
    k230.setTrackModel(false);
}

void CarTest::advanceCorrectionTest() {
    k230.setCameraSteeringGearAngle(-80);
    k230.setTrackModel(TRACK_MIDDLE);
    car.advanceCorrection(50, 6);
    k230.setTrackModel(false);
}

K230Test::K230Test() {}


void K230Test::qrRecognizeTest() {
    k230.setCameraSteeringGearAngle(0);
    k230.setTrackModel(0);
    uint8_t c = 0;
    k230.qrRecognize(&c, qrMessageArray, 4);

    Serial.print("[QR Test] QR count:");
    Serial.print(c);
    Serial.println();

    for (int i = 0; i < c; i++) {
        Serial.print("[QR Test] QR #");
        Serial.print(i);

        if (qrMessageArray[i].efficient) {
            Serial.print("  Color: ");
            logObj(qrMessageArray[i].qrColor);

            Serial.print("  Data (ASCII): ");
            for (int j = 0; j < qrMessageArray[i].messageLen; j++) {
                char c = qrMessageArray[i].message[j];
                Serial.print((c >= 32 && c <= 126) ? c : '?'); // 只打印可打印字符
            }
            Serial.println();
        }
    }
}
