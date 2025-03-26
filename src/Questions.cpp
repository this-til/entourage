//
// Created by til on 2025/3/20.
//

#include "Questions.h"


Questions questions;

uint16_t path[32] = {};
uint16_t forecastPath[32] = {};
uint8_t qrBuf[24] = {};
uint8_t qrBuf2[24] = {};
uint8_t licensePlateBuf[7] = {};

/***
 * 无线充电开启码
 */
uint8_t wirelessChargingOpenCode[3] = {};


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

    excludeSpecialCharacter(qrMessageArray[0].message, qrMessageArray[0].messageLen, qrBuf, 24, &qr1Len);
    excludeSpecialCharacter(qrMessageArray[1].message, qrMessageArray[1].messageLen, qrBuf, 24, &qr2Len);

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

    k210.setTrackModel(0);
    k210.setCameraSteeringGearAngle(0);
    trafficLightA.requestToEnterRecognitionMode();

    //识别红绿灯
    K210Color trafficLightColor;
    k210.trafficLightRecognize_rigorous(&trafficLightColor);
    trafficLightA.requestConfirmationOfIdentificationResults(k230ColorToTrafficLightModel(trafficLightColor)); // 任务8

    k210.setCameraSteeringGearAngle(-55);
    k210.setTrackModel(TRACK_MIDDLE);

    car.trackAdvanceToNextJunction(); // F6 -> F4
    car.trackAdvanceToNextJunction(); // F4 -> F2

    car.trackTurnRight(); //lock r


    //识别二维码
    k210.setCameraSteeringGearAngle(0);
    k210.setTrackModel(0);

    uint8_t qrCount;
    k210.qrRecognize(&qrCount, qrMessageArray, 3);

    uint16_t qr1Len = 0;
    uint16_t qr2Len = 0;

    excludeSpecialCharacter(qrMessageArray[0].message, qrMessageArray[0].messageLen, qrBuf, 24, &qr1Len);
    excludeSpecialCharacter(qrMessageArray[1].message, qrMessageArray[1].messageLen, qrBuf, 24, &qr2Len);

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

void Questions::questions2023() {

    /*uint8_t len = 0;
    while (!netSynchronization.getPathInformation(path, 32, &len)) {
        yield();
    }

    uint16_t initiation = path[0];*/

#if HAVE_GARAGE

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
            extractCharacter(qm->message, qm->messageLen, qrBuf, 24, &len, '<', '>', false);
            if (inclusiveCharacter(qrBuf, len, (uint8_t*) "A", 1)) {
                qa = qm;
            }
            if (inclusiveCharacter(qrBuf, len, (uint8_t*) "B", 1)) {
                qb = qm;
            }
        }

        if (qa != nullptr && qb != nullptr) {

            uint16_t qaLen = 0;
            uint16_t qbLen = 0;
            extractCharacter(qa->message, qa->messageLen, qrBuf, 24, &qaLen, '<', '>', true);
            extractCharacter(qb->message, qb->messageLen, qrBuf2, 24, &qbLen, '<', '>', true);

            uint16_t computationalCacheLen;
            spotTheDifference(qrBuf, qrBuf2, min(qaLen, qbLen), universalCache[0], 24, &computationalCacheLen);

            asciiToHex(universalCache[1], computationalCacheLen, wirelessChargingOpenCode, 3, nullptr);

        }


        k210.setCameraSteeringGearAngle(-55);
        k210.setTrackModel(TRACK_MIDDLE);
    }


    car.trackTurnRight(); // look d
    car.trackTurnRight(); // look l

    car.trackAdvanceToNextJunction(); // F2 -> D2

    //TODO 智能显示标志物

    car.overspecificRelief(); // 特殊地形 D2 -> C2
    car.trackAdvanceToNextJunction(); // C2 -> B2

    // TODO 多功能A

    netSynchronization.synchronousGlobalVariable('2', 1);

    //等待A车到达避让点
    while (netSynchronization.getGlobalVariable('2')) {
        yield();
    }

    car.trackTurnLeft(); // look d

    //TODO 交通灯A

    car.trackAdvanceToNextJunction(); // B2 -> B4

    //TODO 交通灯B

    car.trackAdvanceToNextJunction(); // B4 -> B6

    netSynchronization.synchronousGlobalVariable('1', 0);

    //静态A
    {
        k210.setCameraSteeringGearAngle(0);
        k210.setTrackModel(0);


        uint8_t c = 0;
        k210.qrRecognize(&c, qrMessageArray, 4);

        QrMessage* redQr;
        QrMessage* greenQr;

        for (int i = 0; i < c; ++i) {
            QrMessage* qe = qrMessageArray + i;
            if (qe->qrColor == K_RED) {
                redQr = qe;
            }
            if (qe->qrColor == K_GREEN) {
                greenQr = qe;
            }
        }

        if (redQr != nullptr && greenQr != nullptr) {

            uint16_t redQrLen = 0;
            uint16_t greenQrLen = 0;

            clearPreferredMatch();
            preferredMatch['*'] = 1;
            preferredMatch['|'] = 1;
            preferredMatch['!'] = 1;
            preferredMatch['#'] = 1;
            preferredMatch['%'] = 1;
            preferredMatch['<'] = 1;
            preferredMatch['>'] = 1;
            preferredMatch[','] = 1;
            preferredMatch['@'] = 1;
            preferredMatch['&'] = 1;

            excludeCharacter(redQr->message, redQr->messageLen, universalCache[0], UNIVERSAL_CACHE_DATA_LEN, &redQrLen, preferredMatch);


            bool tag = true;

            uint16_t sourceCacheLen = 0;
            uint16_t outCacheLen = redQrLen;

            uint8_t* sourceCache = nullptr;
            uint8_t* outCache = nullptr;


            bool containsMarkers = false;
            const int MAX_LOOPS = 10; // 防止无限循环
            int loopCount = 0;

            do {

                sourceCacheLen = outCacheLen;

                // 确保缓存区切换正确
                sourceCache = universalCache[tag ? 0 : 1];
                outCache = universalCache[tag ? 1 : 0];
                tag = !tag;

                // 安全提取字符，确保不越界
                extractCharacter(sourceCache, sourceCacheLen, outCache, UNIVERSAL_CACHE_DATA_LEN, &outCacheLen);

                // 检查是否仍需继续处理
                containsMarkers = inclusiveCharacter(outCache, outCacheLen, (uint8_t*) "<>", 2, nullptr);

                loopCount++;
                if (loopCount >= MAX_LOOPS) {
                    break; // 达到最大循环次数，强制退出
                }

            } while (containsMarkers);

            //extractCharacter(redQr->message, redQr->messageLen, qrBuf, 24, &redQrLen, '<', '>', false);




        }


        k210.setCameraSteeringGearAngle(-55);
        k210.setTrackModel(TRACK_MIDDLE);
    }

    //TODO 智能报警台

    car.trackTurnLeft(); // look r

    // TODO 智能匝道

    car.trackAdvanceToNextJunction(); // B6 -> D6

    car.trackTurnLeft(); // look u


    // TODO 立体车库A

#if HAVE_CDCSERIAL

    //TODO 倒车入库

#else

    car.advanceCorrection(50, 6);
    car.recoilToNextJunction();
    car.recoil(200);

#endif

}

Questions::Questions() {

}

