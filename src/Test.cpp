//
// Created by til on 2025/3/11.
//

#include "Test.h"


CarTest carTest;

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
        car.trackTurnLeft();
        car.recoilToNextJunction();
        car.advance(300);
        car.rightCar();


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