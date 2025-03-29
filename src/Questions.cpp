//
// Created by til on 2025/3/20.
//

#include "Questions.h"

#define HAVE_GARAGE_A true
#define HAVE_GARAGE_B false

Questions questions;

uint16_t path[32] = {};
uint16_t forecastPath[32] = {};

/***
 * 无线充电开启码
 */
uint8_t wirelessChargingOpenCode[3] = {};

Str wirelessChargingOpenCodeStr = {
        wirelessChargingOpenCode,
        0,
        3
};

uint8_t alarmSwitchCode[6] = {};
bool alarmSwitchCodeOccupy[6] = {};

uint8_t licensePlateNumber[6] = {};

Str licensePlateNumberStr = {
        licensePlateNumber,
        0,
        6
};

Questions::Questions() {
}


/*
void Questions::questions5() {
    uint8_t len = 0;
    while (!netSynchronization.getPathInformation(path, 32, &len)) {
        yield();
    }

    uint16_t initiation = path[0];

    k210.setCameraSteeringGearAngle(-55);
    k210.setTrackModel(TRACK_MIDDLE);

    car.trackAdvanceToNextJunction(); //F1->F2
    car.trackTurnRight();
    car.trimCar();

    k210.setTrackModel(0);
    k210.setCameraSteeringGearAngle(0);
    trafficLightA.requestToEnterRecognitionMode();

    K210Color trafficLightColor;
    k210.trafficLightRecognize_rigorous(&trafficLightColor);
    trafficLightA.requestConfirmationOfIdentificationResults(k230ColorToTrafficLightModel(trafficLightColor)); // 任务8

    k210.setCameraSteeringGearAngle(-55);
    k210.setTrackModel(TRACK_MIDDLE);

    car.trackAdvanceToNextJunction(); //F2->D2

    car.trackAdvanceToNextJunction(); //D2->B2

    k210.setTrackModel(0);
    k210.setCameraSteeringGearAngle(0);

    //二维码
    uint8_t qrCount;
    k210.qrRecognize(&qrCount, qrMessageArray, 4);


    uint16_t qr1Len = 0;
    uint16_t qr2Len = 0;

    excludeSpecialCharacter(qrMessageArray[0].str, qrBuf);
    excludeSpecialCharacter(qrMessageArray[1].str, qrBuf);

    uint8_t qr1SpecialNumber = qrMessageArray[0].messageLen - qr1Len;
    uint8_t qr2SpecialNumber = qrMessageArray[0].messageLen - qr2Len;

    QrMessage* qr1Message;
    QrMessage* qr2Message;

    if (qr1SpecialNumber < qr2SpecialNumber) {
        qr1Message = qrMessageArray;
        qr2Message = qrMessageArray + 1;
    } else {
        qr1Message = qrMessageArray + 1;
        qr2Message = qrMessageArray;
    }

    excludeSpecialCharacter(qr1Message->message, qr1Message->messageLen, licensePlateBuf, 6, nullptr);

    netSynchronization.synchronousLicensePlateNumber(licensePlateBuf);


    k210.setCameraSteeringGearAngle(-55);
    k210.setTrackModel(TRACK_MIDDLE);

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

    Direction endDirection;
    for (int i = 1; i < len - 1; ++i) {
        sandTable.moveTo(pack(path[i]), D_UNDER, pack(path[i + 1]), &endDirection);
        if (path[i + 1] == assembly("B6")) {
            CoreBeep.TurnOn();
            sleep(1000);
            CoreBeep.TurnOff();
            netSynchronization.synchronousGlobalVariable('q', 0);
        }
    }

    sandTable.adjustDirection(pack(assembly("D6")), endDirection, D_UP);

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
*/

void Questions::questions10() {
    uint8_t len = 0;
    unsigned long startTime = millis();
    unsigned long outTime = 2l * 60l * 1000l;
    while (true) {
        if (netSynchronization.getPathInformation(path, 32, &len)) {
            break;
        }
        if (millis() - startTime > outTime) {
            break;
        }
        yield();
    }

    uint16_t initiation = path[0];
/*    initiation = assembly("F7");*/

    k210.setCameraSteeringGearAngle(-55);
    k210.setTrackModel(TRACK_MIDDLE);

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

    //红绿灯A
    {
        k210.setTrackModel(0);
        k210.setCameraSteeringGearAngle(15);

        trafficLightA.requestToEnterRecognitionMode();
        sleep(1000);
        K210Color trafficLightColor;
        k210.trafficLightRecognize(&trafficLightColor);
        trafficLightA.requestConfirmationOfIdentificationResults(k230ColorToTrafficLightModel(trafficLightColor)); // 任务8

        k210.setCameraSteeringGearAngle(-55);
        k210.setTrackModel(TRACK_MIDDLE);
    }

    car.trackTurnRight(); //lock r

    //静态A
    {
        k210.setTrackModel(0);
        k210.setCameraSteeringGearAngle(15);

        uint8_t qrCount;
        k210.qrRecognize(&qrCount, qrMessageArray, 3);

        k210.setCameraSteeringGearAngle(-55);
        k210.setTrackModel(TRACK_MIDDLE);
    }

    car.trackTurnLeft();

    car.trackAdvanceToNextJunction(); // F6 -> F4
    car.trackAdvanceToNextJunction(); // F4 -> F2

    car.trackTurnRight(); //lock r
    car.trimCar();


    QrMessage* qr1Message;
    QrMessage* qr2Message;

    //静态B
    {
        k210.setCameraSteeringGearAngle(0);
        k210.setTrackModel(0);

        uint8_t qrCount;
        k210.qrRecognize(&qrCount, qrMessageArray, 3);


        excludeSpecialCharacter(&qrMessageArray[0].str, universalCache + 0);
        excludeSpecialCharacter(&qrMessageArray[1].str, universalCache + 1);

        uint16_t qr1Len = universalCache[0].len;
        uint16_t qr2Len = universalCache[1].len;

        uint8_t qr1SpecialNumber = qrMessageArray[0].str.len - qr1Len;
        uint8_t qr2SpecialNumber = qrMessageArray[0].str.len - qr2Len;


        if (qr1SpecialNumber < qr2SpecialNumber) {
            qr1Message = qrMessageArray;
            qr2Message = qrMessageArray + 1;
        } else {
            qr1Message = qrMessageArray + 1;
            qr2Message = qrMessageArray;
        }

        excludeSpecialCharacter(&qr1Message->str, &licensePlateNumberStr);

        netSynchronization.synchronousLicensePlateNumber(licensePlateNumberStr.str);

        k210.setCameraSteeringGearAngle(-55);
        k210.setTrackModel(TRACK_MIDDLE);
    }

    car.trackTurnLeft(); // lock u
    car.trackTurnLeft(); // lock l

    car.turnLeft(car.turningSpeed, 45); // lock ld
    sleep(500);
    stereoscopicDisplayA.showLicensePlate(licensePlateNumberStr.str, 0, 0);
    sleep(500);
    car.turnRight(car.turningSpeed, 30); // lock l
    car.trimCar();

    //红绿灯B
    {
        k210.setTrackModel(0);
        k210.setCameraSteeringGearAngle(0);

        trafficLightB.requestToEnterRecognitionMode();
        sleep(1000);
        K210Color trafficLightColor;
        k210.trafficLightRecognize(&trafficLightColor);
        trafficLightB.requestConfirmationOfIdentificationResults(k230ColorToTrafficLightModel(trafficLightColor)); // 任务8

        k210.setCameraSteeringGearAngle(-55);
        k210.setTrackModel(TRACK_MIDDLE);
    }

    //自动寻迹

    uint16_t* _path = path + 2;
    uint8_t _len = len - 3;


    assembly("D2", forecastPath, _len);
    if (equals(_path, forecastPath, _len)) {

        car.trackAdvanceToNextJunction(); // F2 -> D2

        startTime = millis();
        while (millis() - startTime < 10000) {
            barrierGateA.setLicensePlateData(licensePlateNumberStr.str);
            CoreBeep.TurnOn();
            sleep(100);
            CoreBeep.TurnOff();
            if (barrierGateA.getGateControl()) {
                break;
            }
        }

        car.trackAdvanceToNextJunction(); // D2 -> B2
        car.trackTurnLeft(); // lock d;
    }

#if HAVE_GARAGE_A
    uint8_t level;
    carportA.getLevel(&level);
    netSynchronization.synchronousGlobalVariable('y', level);
#else
    netSynchronization.synchronousGlobalVariable('y', 2);
#endif

    uint16_t x = evaluateTheExpression(qr2Message->str.str, netSynchronization.globalVariable);
    netSynchronization.synchronousGlobalVariable('x', x);

#if HAVE_GARAGE_A

    car.reverseIntoTheCarport(&carportA, 3);

#else
    car.advanceCorrection(50, 6);
    car.recoilToNextJunction();
    car.recoil(200);
#endif

    netSynchronization.synchronousGlobalVariable('q', 0);

}

void Questions::questions2023() {

    /*uint8_t len = 0;
    while (!netSynchronization.getPathInformation(path, 32, &len)) {
        yield();
    }

    uint16_t initiation = path[0];*/

#if HAVE_GARAGE_B

    carportB.moveToLevel(1);
    uint8_t  level;
    while (true) {
        carportB.getLevel( & level);
        if (level == 1) {
            break;
        }
    }

#endif


    k210.setCameraSteeringGearAngle(-55);
    k210.setTrackModel(TRACK_MIDDLE);

    // 触发F1 look d

    car.trackAdvanceToNextJunction(); // F1 -> F2

    car.trackTurnLeft(); // look r
    car.trimCar();

    // 静态B
    {
        k210.setCameraSteeringGearAngle(0);
        k210.setTrackModel(0);

        uint8_t c = 0;
        k210.qrRecognize(&c, qrMessageArray, 4);

        // TODO 查找蓝色

        bool isRed = true;
        bool isGreen = true;

        /*QrMessage* redQr;
        QrMessage* greenQr;*/


        for (int i = 0; i < c; ++i) {
            K210Color k210Color = qrMessageArray[i].qrColor;
            if (k210Color == K_RED) {
                isRed = false;
            }
            if (k210Color == K_GREEN) {
                isGreen = false;
            }

        }

        uint8_t y;

        if (isRed) {
            y = 1;
        } else if (isGreen) {
            y = 2;
        } else {
            y = 3; // 蓝色
        }

        netSynchronization.synchronousGlobalVariable('y', y);

        QrMessage* qa = nullptr;
        QrMessage* qb = nullptr;

        for (int i = 0; i < c; ++i) {
            QrMessage* qm = qrMessageArray + i;
            uint16_t len = 0;
            extractCharacter(&qm->str, universalCache + 0, extra);
            if (inclusiveCharacter(universalCache + 0, (uint8_t*) "A", 1)) {
                qa = qm;
            }
            if (inclusiveCharacter(universalCache + 0, (uint8_t*) "B", 1)) {
                qb = qm;
            }
        }

        if (qa != nullptr && qb != nullptr) {

            uint16_t qaLen = 0;
            uint16_t qbLen = 0;
            extractCharacter(&qa->str, universalCache + 1, shallowest);
            extractCharacter(&qb->str, universalCache + 2, shallowest);

            uint16_t computationalCacheLen;
            spotTheDifference(universalCache + 0, universalCache + 1, universalCache + 2);

            asciiToHex(universalCache + 2, &wirelessChargingOpenCodeStr);

        }


        k210.setCameraSteeringGearAngle(-55);
        k210.setTrackModel(TRACK_MIDDLE);
    }


    car.trackTurnRight(); // look d
    car.trackTurnRight(); // look l

    car.trackAdvanceToNextJunction(); // F2 -> D2

    //TODO 智能显示标志物

    car.recoil(400);
    car.overspecificRelief(); // 特殊地形 D2 -> C2
    car.trackAdvanceToNextJunction(); // C2 -> B2

    // TODO 多功能A

    netSynchronization.synchronousGlobalVariable('2', 1);

    //等待A车到达避让点
    while (netSynchronization.getGlobalVariable('2')) {
        yield();
    }

    car.trackTurnLeft(); // look d
    car.trimCar();

    //TODO 交通灯A6
    {
        k210.setCameraSteeringGearAngle(15);
        k210.setTrackModel(0);

        K210Color k210Color = K_GREEN;
        trafficLightA.requestToEnterRecognitionMode();
        k210.trafficLightRecognize_rigorous(&k210Color);
        trafficLightA.requestConfirmationOfIdentificationResults(k230ColorToTrafficLightModel(k210Color));

        k210.setCameraSteeringGearAngle(-55);
        k210.setTrackModel(TRACK_MIDDLE);

    }


    car.trackAdvanceToNextJunction(); // B2 -> B4


    //TODO 交通灯B
    {
        k210.setCameraSteeringGearAngle(15);
        k210.setTrackModel(0);

        K210Color k210Color = K_GREEN;
        trafficLightB.requestToEnterRecognitionMode();
        k210.trafficLightRecognize_rigorous(&k210Color);
        trafficLightB.requestConfirmationOfIdentificationResults(k230ColorToTrafficLightModel(k210Color));


        k210.setCameraSteeringGearAngle(-55);
        k210.setTrackModel(TRACK_MIDDLE);
    }


    car.trackAdvanceToNextJunction(); // B4 -> B6

    //结束避让
    netSynchronization.synchronousGlobalVariable('1', 0);

    car.trackTurnRight(); // look l
    car.trimCar();

    //静态A
    {
        k210.setCameraSteeringGearAngle(0);
        k210.setTrackModel(0);


        uint8_t qrCount = 0;
        k210.qrRecognize(&qrCount, qrMessageArray, 4);

        QrMessage* redQr;
        QrMessage* greenQr;

        for (int i = 0; i < qrCount; ++i) {
            QrMessage* qe = qrMessageArray + i;
            if (qe->qrColor == K_RED) {
                redQr = qe;
            }
            if (qe->qrColor == K_GREEN) {
                greenQr = qe;
            }
        }

        if (redQr != nullptr && greenQr != nullptr) {


            uint8_t qPos = 0;
            uint8_t wPos = 0;

            Str* str = universalCache + 0;

            extractCharacter(&redQr->str, str, deepest);

            bool tag = true;
            for (int i = 0; i < str->len; ++i) {
                if (str->str[i] == ',') {
                    continue;
                }
                if (tag) {
                    qPos = str->str[i] - '0';
                    tag = false;
                } else {
                    wPos = str->str[i] - '0';
                    break;
                }
            }

            Str* peripheryStr = universalCache + 1;
            extractCharacter(&redQr->str, peripheryStr, extra);


            clearPreferredMatch();
            preferredMatch['*'] = true;
            preferredMatch['|'] = true;
            preferredMatch['!'] = true;
            preferredMatch['#'] = true;
            preferredMatch['%'] = true;
            preferredMatch['<'] = true;
            preferredMatch['>'] = true;
            preferredMatch[','] = true;
            preferredMatch['@'] = true;
            preferredMatch['&'] = true;

            Str* purePeripheryStr = universalCache + 2;

            excludeCharacter(peripheryStr, purePeripheryStr, preferredMatch);

            tag = true;
            for (int i = 0; i < purePeripheryStr->len; ++i) {
                uint8_t c = purePeripheryStr->str[i];
                if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')) {
                    if (tag) {
                        alarmSwitchCode[qPos] = c;
                        alarmSwitchCodeOccupy[qPos] = true;
                        tag = false;
                    } else {
                        alarmSwitchCode[wPos] = c;
                        alarmSwitchCodeOccupy[wPos] = true;
                    }
                }
            }


            extractCharacter(&greenQr->str, str, deepest);

            uint16_t mPos;
            uint16_t nPos;

            tag = true;
            for (int i = 0; i < str->len; ++i) {
                if (str->str[i] == ',') {
                    continue;
                }
                if (tag) {
                    mPos = str->str[i] - '0';
                    tag = false;
                } else {
                    nPos = str->str[i] - '0';
                    break;
                }
            }

            extractCharacter(&redQr->str, peripheryStr, extra);

            excludeCharacter(peripheryStr, purePeripheryStr, preferredMatch);

            tag = true;
            for (uint16_t i = 0; i < purePeripheryStr->len; ++i) {
                uint8_t c = purePeripheryStr->str[i];
                if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')) {
                    if (tag) {
                        alarmSwitchCode[mPos] = c;
                        alarmSwitchCodeOccupy[mPos] = true;
                        tag = false;
                    } else {
                        alarmSwitchCode[nPos] = c;
                        alarmSwitchCodeOccupy[nPos] = true;
                    }
                }
            }

        }


        k210.setCameraSteeringGearAngle(-55);
        k210.setTrackModel(TRACK_MIDDLE);
    }

    car.trackTurnLeft(); //look d

    //智能报警台
    {

        uint8_t x = netSynchronization.getGlobalVariable('x');
        uint8_t z = netSynchronization.getGlobalVariable('z');
        bool tag = true;
        for (int i = 0; i < 6; ++i) {
            if (alarmSwitchCodeOccupy[i]) {
                continue;
            }
            if (tag) {
                alarmSwitchCode[i] = x;
                tag = false;
            } else {
                alarmSwitchCode[i] = z;
            }
        }
        alarmDeskA.openByInfrared(alarmSwitchCode);
    }

    car.trackTurnLeft(); // look r

    // 智能匝道
    {
        while (!netSynchronization.getLicensePlateNumber(licensePlateNumber)) {
            yield();
        }
        barrierGateA.sendLicensePlateDataAndWaitOpen(licensePlateNumber);


    }
    car.trackAdvanceToNextJunction(); // B6 -> D6

    car.trackTurnLeft(); // look u


#if HAVE_GARAGE_A

    car.reverseIntoTheCarport(&carportA, netSynchronization.getGlobalVariable('y'));

#else

    car.advanceCorrection(50, 6);
    car.recoilToNextJunction();
    car.recoil(200);

#endif

}


