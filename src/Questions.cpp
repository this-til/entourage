//
// Created by til on 2025/3/20.
//

#include "Questions.h"


Questions questions;

uint16_t path[32] = {};
uint16_t forecastPath[32] = {};
uint8_t qrBuf[48] = {};
uint8_t licensePlateBuf[7] = {};

void Questions::questions5() {
    uint8_t len = 0;
    while (!netSynchronization.getPathInformation(path, 32, &len)) {
        yield();
    }

    uint16_t initiation = path[0];

    k230.setCameraSteeringGearAngle(-55);
    k230.setTrackModel(TRACK_MIDDLE);

    car.trackAdvanceToNextJunction(); //F1->F2
    car.trackTurnRight();
    car.trimCar();

    k230.setTrackModel(0);
    k230.setCameraSteeringGearAngle(0);
    trafficLightA.requestToEnterRecognitionMode();

    K210Color trafficLightColor;
    k230.trafficLightRecognize_rigorous(&trafficLightColor);
    trafficLightA.requestConfirmationOfIdentificationResults(k230ColorToTrafficLightModel(trafficLightColor)); // 任务8

    k230.setCameraSteeringGearAngle(-55);
    k230.setTrackModel(TRACK_MIDDLE);

    car.trackAdvanceToNextJunction(); //F2->D2

    car.trackAdvanceToNextJunction(); //D2->B2

    k230.setTrackModel(0);
    k230.setCameraSteeringGearAngle(0);

    //二维码
    uint8_t qrCount;
    k230.qrRecognize(&qrCount, qrMessageArray, 4);


    uint8_t qr1SpecialNumber = 0;
    uint8_t qr2SpecialNumber = 0;

    excludeSpecialCharacter(qrMessageArray[0].message, qrMessageArray[0].messageLen, qrBuf, 48, &qr1SpecialNumber);
    excludeSpecialCharacter(qrMessageArray[1].message, qrMessageArray[1].messageLen, qrBuf, 48, &qr1SpecialNumber);

    QrMessage* qr1Message;
    QrMessage* qr2Message;

    if (qr1SpecialNumber < qr2SpecialNumber) {
        qr1Message = qrMessageArray;
        qr2Message = qrMessageArray + 1;
    } else {
        qr1Message = qrMessageArray + 1;
        qr2Message = qrMessageArray;
    }

    excludeSpecialCharacter(qr1Message->message, qr1Message->messageLen, licensePlateBuf, 6);

    netSynchronization.synchronousLicensePlateNumber(licensePlateBuf);


    k230.setCameraSteeringGearAngle(-55);
    k230.setTrackModel(TRACK_MIDDLE);

    car.trackTurnLeft();

    unsigned long startTime = millis();
    while (millis() - startTime < 10000) {
        barrierGateA.setLicensePlateData(licensePlateBuf);
        CoreBeep.TurnOn();
        sleep(1000);
        CoreBeep.TurnOff();
        if (barrierGateA.getGateControl()) {
            break;
        }
    }

    car.trackAdvanceToNextJunction(); //B2->B4
    car.trimCar();
    car.turnLeft(40, 45);
    sleep(500);
    stereoscopicDisplayA.showLicensePlate(licensePlateBuf, 0, 0);
    sleep(500);
    car.turnRight(40, 45);
    car.trimCar();

    uint16_t* _path = path + 2;
    uint8_t _len = len - 3;


    assembly("D4", forecastPath, _len);
    if (equals(_path, forecastPath, _len)) {
        car.trackTurnLeft();
        car.trackAdvanceToNextJunction(); //B4->D4

        netSynchronization.synchronousGlobalVariable('q', 0);
        CoreBeep.TurnOn();
        sleep(1000);
        CoreBeep.TurnOff();

        car.trackTurnRight();

        car.trackAdvanceToNextJunction(); //D4->D6

        car.trackTurnRight();
        car.trackTurnRight();
    }

    assembly("B6", forecastPath, _len);
    if (equals(_path, forecastPath, _len)) {
        car.trackAdvanceToNextJunction(); //B4->B6
        netSynchronization.synchronousGlobalVariable('q', 0);
        CoreBeep.TurnOn();
        sleep(1000);
        CoreBeep.TurnOff();

        car.turnLeft();
        car.recoil(500);
        car.trimCar();
        car.advance(50);
        car.overspecificRelief();
        car.trackAdvanceToNextJunction(); //B6->D6
        car.turnLeft();
    }

    /* Direction endDirection;
     for (int i = 1; i < len - 1; ++i) {
         sandTable.moveTo(pack(path[i]), D_UNDER, pack(path[i + 1]), &endDirection);
         if (path[i + 1] == assembly("B6")) {
             CoreBeep.TurnOn();
             sleep(1000);
             CoreBeep.TurnOff();
             netSynchronization.synchronousGlobalVariable('q', 0);
         }
     }

     sandTable.adjustDirection(pack(assembly("D6")), endDirection, D_UP);*/

#if HAVE_GARAGE

    uint8_t level;
    carportA.getLevel(&level);
    netSynchronization.synchronousGlobalVariable('h', level);

    // TODO
#else

    netSynchronization.synchronousGlobalVariable('h', 2);
    car.advanceCorrection(50, 6);
    car.recoilToNextJunction();

#endif

}

void Questions::questions10() {
    uint8_t len = 0;
    while (!netSynchronization.getPathInformation(path, 32, &len)) {
        yield();
    }

    uint16_t initiation = path[0];

    k230.setCameraSteeringGearAngle(-55);
    k230.setTrackModel(TRACK_MIDDLE);

    if (initiation == assembly("D7")) {
        car.trackAdvanceToNextJunction(); //D7 -> D6
        car.trackTurnRight(); // look r
        car.trackAdvanceToNextJunction(); //D6 -> F6
        car.turnLeft(); // look u
    }

    if (initiation == assembly("F7")) {
        car.trackAdvanceToNextJunction(); //F7 -> F6
    }

    if (initiation == assembly("D5")) {
        car.turnRight(car.turningSpeed, 180); // look d
        car.trimCar();

        car.trackAdvanceToNextJunction(); //D5 -> D6
        car.trackTurnRight(); // lock r
        car.trackAdvanceToNextJunction(); //D6 -> F6
        car.turnLeft(); // look u
    }

    k230.setTrackModel(0);
    k230.setCameraSteeringGearAngle(0);
    trafficLightA.requestToEnterRecognitionMode();

    //识别红绿灯
    K210Color trafficLightColor;
    k230.trafficLightRecognize_rigorous(&trafficLightColor);
    trafficLightA.requestConfirmationOfIdentificationResults(k230ColorToTrafficLightModel(trafficLightColor)); // 任务8

    k230.setCameraSteeringGearAngle(-55);
    k230.setTrackModel(TRACK_MIDDLE);

    car.trackAdvanceToNextJunction(); // F6 -> F4
    car.trackAdvanceToNextJunction(); // F4 -> F2

    car.trackTurnRight(); //lock r


    //识别二维码
    k230.setCameraSteeringGearAngle(0);
    k230.setTrackModel(0);

    uint8_t qrCount;
    k230.qrRecognize(&qrCount, qrMessageArray, 4);

    uint8_t qr1SpecialNumber = 0;
    uint8_t qr2SpecialNumber = 0;

    excludeSpecialCharacter(qrMessageArray[0].message, qrMessageArray[0].messageLen, qrBuf, 48, &qr1SpecialNumber);
    excludeSpecialCharacter(qrMessageArray[1].message, qrMessageArray[1].messageLen, qrBuf, 48, &qr1SpecialNumber);

    QrMessage* qr1Message;
    QrMessage* qr2Message;

    if (qr1SpecialNumber < qr2SpecialNumber) {
        qr1Message = qrMessageArray;
        qr2Message = qrMessageArray + 1;
    } else {
        qr1Message = qrMessageArray + 1;
        qr2Message = qrMessageArray;
    }

    excludeSpecialCharacter(qr1Message->message, qr1Message->messageLen, licensePlateBuf, 6);

    netSynchronization.synchronousLicensePlateNumber(licensePlateBuf);

    k230.setCameraSteeringGearAngle(-55);
    k230.setTrackModel(TRACK_MIDDLE);

    car.trackTurnLeft(); // lock u
    car.trackTurnLeft(); // lock l

    car.turnLeft(car.turningSpeed, 60); // lock ld
    sleep(500);
    stereoscopicDisplayA.showLicensePlate(licensePlateBuf, 0, 0);
    sleep(500);
    car.turnRight(car.turningSpeed, 60); // lock l
    car.trimCar();

//自动寻迹


    uint16_t* _path = path + 2;
    uint8_t _len = len - 3;


    assembly("D2", forecastPath, _len);
    if (equals(_path, forecastPath, _len)) {

        car.trackAdvanceToNextJunction(); // F2 -> D2

        unsigned long startTime = millis();
        while (millis() - startTime < 10000) {
            barrierGateA.setLicensePlateData(licensePlateBuf);
            CoreBeep.TurnOn();
            sleep(1000);
            CoreBeep.TurnOff();
            if (barrierGateA.getGateControl()) {
                break;
            }
        }

        car.trackAdvanceToNextJunction(); // D2 -> B2
        car.trackTurnLeft(); // lock d;
    }

#if HAVE_GARAGE

    uint8_t level;
    carportA.getLevel(&level);
    netSynchronization.synchronousGlobalVariable('y', level);

#else

    netSynchronization.synchronousGlobalVariable('y', 2);


#endif

    uint16_t x = evaluateTheExpression(qr2Message->message, netSynchronization.globalVariable);
    netSynchronization.synchronousGlobalVariable('x', x);

#if HAVE_GARAGE
    //TODO
#else
    car.advanceCorrection(50, 6);
    car.recoilToNextJunction();
    car.recoil(200);
#endif

    netSynchronization.synchronousGlobalVariable('q', 0);

}

Questions::Questions() {

}

