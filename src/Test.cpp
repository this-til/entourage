//
// Created by til on 2025/3/11.
//

#include "Test.h"


CarTest carTest;
K210Test k210Test;

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
    k210.setCameraSteeringGearAngle(-50);
    k210.setTrackModel(TRACK_MIDDLE);
    for (int i = 0; i < 100; ++i) {
        car.trackAdvanceToNextJunction();
        car.turnLeft();

        car.recoil(500);
        car.trimCar();
        //car.recoilToNextJunction();
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
    k210.setTrackModel(0);
}

void CarTest::overspecificReliefTest() {
    k210.setCameraSteeringGearAngle(-50);
    k210.setTrackModel(TRACK_MIDDLE);

    for (int i = 0; i < 100; ++i) {
        car.trackTurnLeft();
        car.trimCar();
        car.turnLeft();
        car.recoil(600);
        car.overspecificRelief();
        car.trackAdvanceToNextJunction();

        car.trackTurnRight();
        car.trimCar();
        car.turnRight();
        car.recoil(600);
        car.overspecificRelief();
        car.trackAdvanceToNextJunction();
    }


    k210.setTrackModel(0);
}

void CarTest::turnLeftTest() {
    k210.setCameraSteeringGearAngle(-55);
    k210.setTrackModel(TRACK_MIDDLE);
    car.turnLeft();
    k210.setTrackModel(false);
}

void CarTest::trackTurnLeftTest() {
    k210.setCameraSteeringGearAngle(-55);
    k210.setTrackModel(TRACK_MIDDLE);
    car.trackTurnLeft();
    k210.setTrackModel(false);
}

void CarTest::trackAdvanceToNextJunctionTest() {
    k210.setCameraSteeringGearAngle(-55);
    k210.setTrackModel(TRACK_MIDDLE);
    car.trackAdvanceToNextJunction();
    k210.setTrackModel(false);
}

void CarTest::mobileCorrectionTest() {
    k210.setCameraSteeringGearAngle(-55);
    k210.setTrackModel(TRACK_MIDDLE);
    car.mobileCorrection(200);
    k210.setTrackModel(false);
}

void CarTest::rightCarTest() {
    k210.setCameraSteeringGearAngle(-55);
    k210.setTrackModel(TRACK_MIDDLE);
    car.rightCar();
    k210.setTrackModel(false);
}

void CarTest::advanceCorrectionTest() {
    k210.setCameraSteeringGearAngle(-55);
    k210.setTrackModel(TRACK_MIDDLE);
    car.advanceCorrection(50, 6);
    k210.setTrackModel(false);
}

void CarTest::reverseIntoTheCarportTest() {
    k210.setCameraSteeringGearAngle(-55);
    k210.setTrackModel(TRACK_MIDDLE);
    car.reverseIntoTheCarport(&carportA, 2);
    k210.setTrackModel(false);
}

void CarTest::trimCarByLineTest() {
    k210.setCameraSteeringGearAngle(-55);
    k210.setTrackModel(TRACK_MIDDLE);
    car.trimCarByLine();
    k210.setTrackModel(false);
}

K210Test::K210Test() {}


void K210Test::qrRecognizeTest() {
    k210.setCameraSteeringGearAngle(0);
    k210.setTrackModel(0);
    uint8_t c = 0;
    k210.qrRecognize(&c, qrMessageArray, QR_CACHE_LEN);

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
            for (int j = 0; j < qrMessageArray[i].str.len; j++) {
                char ch = qrMessageArray[i].str.str[j];
                Serial.print((ch >= 32 && ch <= 126) ? ch : '?'); // 只打印可打印字符
            }
            Serial.println();
        }
    }
}
