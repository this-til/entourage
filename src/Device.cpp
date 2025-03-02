//
// Created by til on 2025/2/27.
//

#include "Device.h"

DeviceBase::DeviceBase(uint8_t id) {
    this->id = id;

    this->sendBus = 0x6008;
    this->sendLen = 8;
    this->sendVerify = true;
    this->sendCooling_ms = 200;

    this->readBus = 0x6100;
    this->readLen = 8;
    this->readVerify = true;
    this->readTailIntegrity = true;
    this->readOutTime_ms = 500;

    this->debug = true;
}

void DeviceBase::send(uint8_t* buf) {
    if (sendVerify) {
        uint32_t com = 0;
        for (uint8_t i = 2; i < sendLen - 2; i++) {
            com += buf[i];
        }
        buf[sendLen - 2] = static_cast<int8_t>(com % 256);
    }
#if DE_BUG
    Serial.print("[DEBUG] send:");
    logHex(buf, sendLen);
    Serial.println();
#endif
    ExtSRAMInterface.ExMem_Write_Bytes(sendBus, buf, sendLen);

    if (sendCooling_ms) {
        sleep(sendCooling_ms);
    }
}

bool DeviceBase::check(const uint8_t* buf, const uint8_t* serviceId, uint8_t serviceLen) {
    if (buf[0] != DATA_FRAME_HEADER) {
#if DE_BUG
        Serial.print("  inaccuracy header:");
        logHex(buf[0]);
#endif
        return false;
    }
    if (readTailIntegrity && buf[readLen - 1] != DATA_FRAME_TAIL) {
#if DE_BUG
        Serial.print("  inaccuracy tail:");
        logHex(buf[readLen - 1]);
        Serial.println("    check failure...");
#endif
        return false;
    }
    if (buf[1] != id) {
#if DE_BUG
        Serial.println("  inaccuracy device:");
        logHex(buf[1]);
        Serial.println("    check failure...");
#endif
        return false;
    }
    for (int i = 0; i < serviceLen; ++i) {
        if (buf[2 + i] != serviceId[i]) {
#if DE_BUG
            Serial.print("  inaccuracy service");
            Serial.print("predict:");
            logHex(serviceId, (uint16_t) serviceLen);
            Serial.print(",source:");
            logHex(buf + 2, (uint16_t) serviceLen);
            Serial.println("    check failure...");
#endif
            return false;
        }
    }

    if (readVerify) {
        uint32_t com = 0;
        for (uint8_t i = 1; i < readLen - 2; i++) {
            com += buf[i];
        }
        auto checksum = uint8_t(com % 256);
        if (buf[readLen - 2] != checksum) {
#if DE_BUG
            Serial.print("  inaccuracy checksum:");
            Serial.print("predict:");
            logHex(checksum);
            Serial.print(",source:");
            logHex(buf[readLen - 2]);
            Serial.println("    check failure...");
#endif
            return false;
        }
    }
#if DE_BUG
    Serial.println("    check successful...");
#endif
    return true;
}


bool DeviceBase::awaitReturn(uint8_t* buf, const uint8_t* serviceId, uint8_t serviceLen) {
    unsigned long startTime = millis();

#if DE_BUG
    Serial.println("[DEBUG] awaitReturn...");
#endif
    while (millis() - startTime < readOutTime_ms) {
        if (ExtSRAMInterface.ExMem_Read(readBus) == 0x00) {
            continue;
        }
        ExtSRAMInterface.ExMem_Read_Bytes(readBus, buf, readLen);
#if DE_BUG
        Serial.print("[DEBUG] read:");
        logHex(buf, readLen);
#endif
        if (check(buf, serviceId, serviceLen)) {
            return true;
        }
    }
#if DE_BUG
    Serial.println("[WARN] outTime...");
#endif
    return false;
}

bool DeviceBase::check(const uint8_t* buf, uint8_t serviceId) {
    return check(buf, &serviceId, 1);
}

bool DeviceBase::awaitReturn(uint8_t* buf, uint8_t serviceId) {
    return awaitReturn(buf, &serviceId, 1);
}


AlarmDesk::AlarmDesk(uint8_t id) : DeviceBase(id) {
}

void AlarmDesk::openByInfrared(uint8_t* openCode) {
    Infrare.Transmition(openCode, 6);
}

/***
   * 获取救援位置
   */
uint8_t AlarmDesk::getRescuePosition() {
    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0x09, 0x00, 0x00, 0x00, 0x00, DATA_FRAME_TAIL};
    Command.Judgment(buf);
    ExtSRAMInterface.ExMem_Write_Bytes(BUS_BASE_ADD, buf, 8);

    if (awaitReturn(buf, 0x01)) {
        return buf[3];
    }
    return 0;
}

/***
 * debug
 * 设置开启码
 * @param data[] len=6
 */
void AlarmDesk::setOpenCode(uint8_t data[]) {

    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0x10, data[5], data[4], data[3], 0x00, DATA_FRAME_TAIL};
    send(buf);

    uint8_t buf2[] = {DATA_FRAME_HEADER, id, 0x11, data[2], data[1], data[0], 0x00, DATA_FRAME_TAIL};
    send(buf2);
}


BarrierGate::BarrierGate(uint8_t id) : DeviceBase(id) {
}

void BarrierGate::setGateControl(bool open) {
    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0x01, static_cast<uint8_t>(open ? 0x01 : 0x02), 0x00, 0x00, 0x00, DATA_FRAME_TAIL};
    send(buf);
}

bool BarrierGate::getGateControl() {
    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0x20, 0x01, 0x00, 0x00, 0x00, DATA_FRAME_TAIL};
    send(buf);

    return awaitReturn(buf, 0x01);;
}

BusStop::BusStop(uint8_t id) : DeviceBase(id) {
}

void BusStop::broadcastSpeech(uint8_t lid) {
    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0x10, lid, 0x00, 0x00, 0x00, DATA_FRAME_TAIL};
    send(buf);
}

void BusStop::broadcastRandomSpeech() {
    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0x20, 0x01, 0x00, 0x00, 0x00, DATA_FRAME_TAIL};
    send(buf);
}

void BusStop::setRtcStartDate(uint8_t yy, uint8_t MM, uint8_t dd) {
    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0x30, yy, MM, dd, 0x00, DATA_FRAME_TAIL};
    send(buf);
}

void BusStop::setRtcStartTime(uint8_t HH, uint8_t mm, uint8_t ss) {
    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0x40, HH, mm, ss, 0x00, DATA_FRAME_TAIL};
    send(buf);
}

bool BusStop::getRtcDate(uint8_t* yy, uint8_t* MM, uint8_t* dd) {
    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0x31, 0x00, 0x00, 0x00, 0x00, DATA_FRAME_TAIL};
    send(buf);
    bool successful = awaitReturn(buf, 0x02);
    if (successful) {
        *yy = buf[3];
        *MM = buf[4];
        *dd = buf[5];
    }
    return successful;
}

bool BusStop::getRtcTime(uint8_t* HH, uint8_t* mm, uint8_t* ss) {
    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0x41, 0x00, 0x00, 0x00, 0x00, DATA_FRAME_TAIL};
    send(buf);
    bool successful = awaitReturn(buf, 0x03);
    if (successful) {
        *HH = buf[3];
        *mm = buf[4];
        *ss = buf[5];
    }
    return successful;
}

void BusStop::setWeatherTemperature(Weather weather, uint8_t temperature) {
    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0x42, (uint8_t) weather, temperature, 0x00, 0x00, DATA_FRAME_TAIL};
    send(buf);
}

bool BusStop::getWeatherTemperature(Weather* weather, uint8_t* temperature) {
    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0x43, 0x00, 0x00, 0x00, 0x00, DATA_FRAME_TAIL};
    send(buf);
    bool successful = awaitReturn(buf, 0x04);
    if (successful) {
        *weather = Weather(buf[3]);
        *temperature = buf[4];
    }
    return successful;
}

void BusStop::startSynthesizingLanguage(const uint8_t* data, uint16_t len, TextEncodingFormat textEncodingFormat) {
    uint16_t size = 5 + len;
    uint16_t dataSize = len + 1;
    auto* buf = new uint8_t[size];

    buf[0] = 0xFD;

    buf[1] = dataSize >> 8;
    buf[2] = dataSize;
    buf[3] = 0x01;
    buf[4] = (uint8_t) textEncodingFormat;

    for (int i = 0; i < len; ++i) {
        buf[5 + i] = data[i];
    }

    ExtSRAMInterface.ExMem_Write_Bytes(BUS_BASE_ADD, buf, size);

}

void BusStop::stopSynthesizingLanguage() {
    uint8_t buf[] = {0xFD, 0x00, 0x01, 0x02};
    ExtSRAMInterface.ExMem_Write_Bytes(BUS_BASE_ADD, buf, 4);
}

void BusStop::pauseSynthesizingLanguage() {
    uint8_t buf[] = {0xFD, 0x00, 0x01, 0x03};
    ExtSRAMInterface.ExMem_Write_Bytes(BUS_BASE_ADD, buf, 4);
}

void BusStop::recoverSynthesizingLanguage() {
    uint8_t buf[] = {0xFD, 0x00, 0x01, 0x04};
    ExtSRAMInterface.ExMem_Write_Bytes(BUS_BASE_ADD, buf, 4);
}

Monitor::Monitor(int id) : DeviceBase(id) {
}

void Monitor::setDisplayData(uint8_t pos, const uint8_t value[]) {
    uint8_t buf[] = {DATA_FRAME_HEADER, id, pos, 0x00, 0x00, 0x00, 0x00, DATA_FRAME_TAIL};
    buf[3] = value[0] << 8 | value[1];
    buf[4] = value[2] << 8 | value[3];
    buf[5] = value[4] << 8 | value[4];
    send(buf);

}

void Monitor::setTimingMode(TimingMode timingMode) {
    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0x03, (uint8_t) timingMode, 0x00, 0x00, 0x00, DATA_FRAME_TAIL};
    send(buf);
}

void Monitor::setDistance(uint16_t distance_mm) {
    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0x04, 0x00, (uint8_t) (distance_mm >> 8), (uint8_t) distance_mm, 0x00, DATA_FRAME_TAIL};
    send(buf);
}

Carport::Carport(uint8_t id) : DeviceBase(id) {
}

void Carport::moveToLevel(uint8_t level) {
    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0x01, level, 0x01, 0x00, 0x00, DATA_FRAME_TAIL};
    send(buf);
}

uint8_t Carport::getLevel() {
    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0x02, 0x01, 0x00, 0x00, 0x00, DATA_FRAME_TAIL};
    send(buf);

    if (awaitReturn(buf, 0x01)) {
        return buf[4];
    }
    return 0;
}

void Carport::getInfraredState(bool* ventral, bool* rearSide) {
    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0x02, 0x02, 0x00, 0x00, 0x00, DATA_FRAME_TAIL};
    send(buf);

    if (awaitReturn(buf, 0x02)) {
        *ventral = buf[4] == 0x01;
        *rearSide = buf[5] == 0x02;
    }
}

TrafficLight::TrafficLight(uint8_t id) : DeviceBase(id) {
}

void TrafficLight::requestToEnterRecognitionMode() {
    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0x01, 0x00, 0x00, 0x00, 0x00, DATA_FRAME_TAIL};
    send(buf);
}

void TrafficLight::requestConfirmationOfIdentificationResults(TrafficLightModel trafficLightModel) {
    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0x02, (uint8_t) trafficLightModel, 0x00, 0x00, 0x00, DATA_FRAME_TAIL};
    send(buf);
}

StreetLamp::StreetLamp(uint8_t id) : DeviceBase(id) {}

K230::K230(uint8_t id) : DeviceBase(id) {
    readBus = 0x6038;
    readVerify = false;

    receiveTrack = true;
    trackFlagBit = 0;
}

void K230::setTrackModel(bool open) {
    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0x91, 0x01, static_cast<uint8_t>(open ? 0x01 : 0x02), 0x00, 0x00, 0x00, DATA_FRAME_TAIL};
    send(buf);
}

void K230::setCameraSteeringGearAngle(int8_t angle) {
    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0x91, 0x02, (uint8_t) angle, 0x00, 0x00, DATA_FRAME_TAIL};
    send(buf);
    //Command.Judgment(buf);
    //ExtSRAMInterface.ExMem_Write_Bytes(BUS_BASE_ADD, buf, 8);
    //Serial.send(buf, 8);
}


uint8_t K230::getTrackFlagBit() {
    return trackFlagBit;
}

bool K230::qrRecognize(uint8_t* count, QrMessage* qrMessageArray, uint8_t maxLen) {
#if DE_BUG
    Serial.println("qrRecognize...");
#endif

    receiveTrack = false;

    for (int i = 0; i < maxLen; ++i) {
        qrMessageArray[i].efficient = false;
    }

    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0x92, 0x01, 0x00, 0x00, 0x00, DATA_FRAME_TAIL};
    send(buf);

    uint8_t serviceIds[] = {0x92, 0x01, 0x01};
    if (!awaitReturn(buf, serviceIds, 3)) {
        *count = 0;
        return false;
    }

    uint8_t s_count = buf[5];

    s_count = s_count > maxLen
              ? maxLen
              : s_count < 0
                ? 0
                : s_count;
    *count = s_count;


#if DE_BUG
    Serial.print("recognize count:");
    logHex(s_count);
    Serial.println();
#endif

    if (s_count == 0) {
        receiveTrack = true;
        return true;
    }

    uint8_t hitCount = 0;
    uint8_t bufLong[32] = {};
    unsigned long startTime = millis();
    while (millis() - startTime < readOutTime_ms) {
        if (ExtSRAMInterface.ExMem_Read(readBus) == 0x00) {
            continue;
        }
        ExtSRAMInterface.ExMem_Read_Bytes(readBus, bufLong, 32);
#if DE_BUG
        Serial.print("read:");
        logHex(bufLong, 32);
        Serial.println();
#endif

        if (
                bufLong[0] != DATA_FRAME_HEADER
                || bufLong[1] != id
                || bufLong[2] != 0x92
                || bufLong[3] != 0x01
                || bufLong[4] != 0x02
                ) {
#if DE_BUG
            Serial.println("verification failure");
#endif
            continue;
        }

        uint8_t qrId = bufLong[5];

#if DE_BUG
        Serial.print("receive qr id:");
        logHex(qrId);
        Serial.println();
#endif

        if (qrId >= maxLen) {
#if DE_BUG
            Serial.println("The id is greater than the quantity, which should not be...");
#endif
            continue;
        }

        QrMessage* qrMessage = qrMessageArray + qrId;

        qrMessage->efficient = true;
        qrMessage->qrColor = K210Color(bufLong[6]);

#if DE_BUG
        Serial.print("receive qr color:");
        logHex(K210Color(bufLong[6]));
        Serial.println();
#endif

        uint8_t messageLen = bufLong[7];
        messageLen = messageLen > qrMessage->messageMaxLen
                     ? qrMessage->messageMaxLen
                     : messageLen < 0
                       ? 0
                       : messageLen;

        qrMessage->messageLen = messageLen;

#if DE_BUG
        Serial.print("message len:");
        logHex(messageLen);
        Serial.println();
#endif

        for (int i = 0; i < messageLen; ++i) {
            qrMessage->message[i] = bufLong[8 + i];
        }

#if DE_BUG
        Serial.print("message:");
        Serial.write(bufLong + 8, messageLen);
        Serial.println();
#endif

        hitCount++;
        if (hitCount >= s_count) {
            break;
        }
    }

    receiveTrack = true;
    return hitCount >= s_count;
}

bool K230::trafficLightRecognize(K210Color* k210Color) {
    receiveTrack = false;

#if DE_BUG
    Serial.println("[DEBUG] trafficLightRecognize...");
#endif

    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0x92, 0x03, 0x00, 0x00, 0x00, DATA_FRAME_TAIL};
    send(buf);

    uint8_t serviceIds[] = {0x92, 0x03};
    bool successful = awaitReturn(buf, serviceIds, 2);

    if (successful) {
        *k210Color = K210Color(buf[4]);
#if DE_BUG
        Serial.print("[DEBUG] traffic light color:");
        logObj(K210Color(buf[4]));
        Serial.println();
#endif
    }

    receiveTrack = true;
    return successful;
}

bool K230::trafficLightRecognize_rigorous(K210Color* k210Color, uint16_t consecutiveEqualDegree, uint16_t maxRetry) {
    receiveTrack = false;

    K210Color currentColor = K_NONE;
    K210Color lastStableColor = K_NONE;
    uint16_t consecutiveCount = 0;

#if DE_BUG
    Serial.print("[DEBUG] Starting rigorous recognition. Requires ");
    Serial.print(consecutiveEqualDegree);
    Serial.print(" consecutive matches. Max retries: ");
    Serial.println(maxRetry);
#endif

    for (uint16_t attempt = 1; attempt <= maxRetry; ++attempt) {
#if DE_BUG
        Serial.print("[DEBUG] Attempt ");
        Serial.print(attempt);
        Serial.print("/");
        Serial.println(maxRetry);
#endif

        if (!trafficLightRecognize(&currentColor)) {
#if DE_BUG
            Serial.println("[WARN] Recognition failed, resetting counter");
#endif
            consecutiveCount = 0;
            lastStableColor = K_NONE;
            continue;
        }

        // 颜色状态机逻辑
        if (currentColor != lastStableColor) {
#if DE_BUG
            Serial.print("[INFO] Color changed from ");
            logObj(lastStableColor);
            Serial.print(" to ");
            logObj(currentColor);
            Serial.println(", resetting counter");
#endif
            lastStableColor = currentColor;
            consecutiveCount = 1;
            continue;
        }

        consecutiveCount++;
#if DE_BUG
        Serial.print("[DEBUG] Consecutive match (");
        Serial.print(consecutiveCount);
        Serial.println(")");
#endif

        // 达到稳定条件
        if (consecutiveCount >= consecutiveEqualDegree) {
#if DE_BUG
            Serial.print("[SUCCESS] Stable color confirmed: ");
            logObj(currentColor);
            Serial.println();
#endif
            *k210Color = currentColor;
            break;
        }
    }

#if DE_BUG
    Serial.println("[ERROR] Failed to reach stable recognition");
#endif
    receiveTrack = true;
    return consecutiveCount >= consecutiveEqualDegree;
}

bool K230::ping() {
    receiveTrack = true;
    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0x92, 0xFE, 0x00, 0x00, 0x00, DATA_FRAME_TAIL};
    send(buf);
    uint8_t serviceIds[] = {0x92, 0xFE};
    bool successful = awaitReturn(buf, serviceIds, 2);
    receiveTrack = false;
    return successful;
}

void K230::loop() {
    if (!receiveTrack) {
        return;
    }

    uint8_t buf[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    if (ExtSRAMInterface.ExMem_Read(readBus) == 0x00) {
        return;
    }

    ExtSRAMInterface.ExMem_Read_Bytes(readBus, buf, 8);

#if DE_BUG
    Serial.print("[DEBUG] k230 loop read:");
    logHex(buf, 8);
#endif
    uint8_t sbuf[] = {0x091, 0x01};
    if (!check(buf, sbuf, 2)) {
        return;
    }

#if DE_BUG
    Serial.print("[DEBUG] trackFlagBit:");
    logHex(buf[4]);
    Serial.println();
#endif

    trackFlagBit = buf[4];


}

/***
 * 到弯停车状态
 */
/*
static const uint8_t TO_BEND_PARKING_CONDITIONS[] = {
        0b11110000, // 0xf0
        0b11100001, // 0xE1
        0b11000011, // 0xC3
        0b10000111, // 0x87
        0b00001111, // 0x0F
        0b10000001, // 0x81
        0b11000000, // 0xC0
        0b00000011, // 0x03
        0b00011111, // 0x1F
        0b11110001, // 0xF1
        0b11111000, // 0xF8
        0b11111100, // 0xFC
        0b00111111, // 0x3F
        0b00000001, // 0x01
        0b10000000, // 0x80
        0b11000001, // 0xC1
        0b10000011, // 0x83
        0b11100000, // 0xE0
        0b00000111,  // 0x07
};
*/

void turnLeft(uint8_t carSpeed) {

}

void turnRight(uint8_t carSpeed) {

}

/***
 * 直行到下一个路口
 * @param carSpeed
 */
void straightLine(uint8_t carSpeed) {

    bool inBend = false;  // 弯道状态标志
    const uint8_t bendThreshold = 6; // 弯道检测阈值（6个以上1）
    const float KP = 15.0f;      // 比例系数（需实际调试）

    DCMotor.SpeedCtr(carSpeed, carSpeed);

    while (true) {
        uint8_t trackFlagBit = k230.getTrackFlagBit();
        uint8_t bitCount = countBits(trackFlagBit);

        if (bitCount >= bendThreshold) {
            inBend = true;
            DCMotor.SpeedCtr(carSpeed, carSpeed);
            return;
        }

        int sum = 0;
        uint8_t validBits = 0;

        for (uint8_t i = 0; i < 8; i++) {
            if (trackFlagBit & (1 << i)) {
                sum += i;
                validBits++;
            }
        }

        if (trackFlagBit == 0) {

        }

        /*if (memchr(TO_BEND_PARKING_CONDITIONS, trackFlagBit, sizeof(TO_BEND_PARKING_CONDITIONS) / sizeof(TO_BEND_PARKING_CONDITIONS[0]))) {
            *//*OpenMV_Stop();
            OpenMV_TrackNewtime(10, 1000);
            OpenMVTrack_Disc_CloseUpNEW();*//*
            break;
        }*/


    }
}

void OpenMV_TrackNew(uint8_t Car_Speed, bool CARZT) {
    // Servo_Control(-45);
    // delay(500);
    uint32_t num = 0;
    float error, left_speed, right_speed;
    // 清空串口缓存
    while (ExtSRAMInterface.ExMem_Read(0x6038) != 0x00) {
        ExtSRAMInterface.ExMem_Read_Bytes(0x6038, Data_OpenMVBuf, 1);
    }
    delay(500);
    //发开始
    OpenMVTrack_Disc_StartUpNEW();
    ExtSRAMInterface.ExMem_Read_Bytes(0x6038, Data_OpenMVBuf, 8);

    DCMotor.SpeedCtr(Car_Speed, Car_Speed);
    while (1) {
        if (ExtSRAMInterface.ExMem_Read(0x6038) != 0x00)  // 检测OpenMV识别结果
        {
            ExtSRAMInterface.ExMem_Read_Bytes(0x6038, Data_OpenMVBuf, 8);
            if (CARZT) {
                if (Data_OpenMVBuf[5] == 0xf0 || Data_OpenMVBuf[5] == 0xE1 || Data_OpenMVBuf[5] == 0xC3 || Data_OpenMVBuf[5] == 0x87 || Data_OpenMVBuf[5] == 0x0F || Data_OpenMVBuf[5] == 0x81 || Data_OpenMVBuf[5] == 0xC0 ||
                    Data_OpenMVBuf[5] == 0X03 || Data_OpenMVBuf[5] == 0X1F || Data_OpenMVBuf[5] == 0XF1 || Data_OpenMVBuf[5] == 0XF8 || Data_OpenMVBuf[5] == 0XFC || Data_OpenMVBuf[5] == 0X3F || Data_OpenMVBuf[5] == 0X01 ||
                    Data_OpenMVBuf[5] == 0X80 || Data_OpenMVBuf[5] == 0XC1 || Data_OpenMVBuf[5] == 0X83 || Data_OpenMVBuf[5] == 0XE0 || Data_OpenMVBuf[5] == 0X07)  // 转弯到了中心
                {
                    OpenMV_Stop();
                    OpenMV_TrackNewtime(10, 1000);

                    OpenMVTrack_Disc_CloseUpNEW();
                    break;
                }
            }
            if ((Data_OpenMVBuf[0] == 0x55) && (Data_OpenMVBuf[1] == 0x02) && (Data_OpenMVBuf[2] == 0x91)) {
                switch (Data_OpenMVBuf[5]) {
                    //偏差大
                    case 0X80:
                        DCMotor.SpeedCtr(0, 80);
                        break;
                    case 0XC0:
                        DCMotor.SpeedCtr(0, 70);
                        break;
                    case 0XE0:
                        DCMotor.SpeedCtr(0, 60);
                        break;
                        //偏差中
                    case 0X60:
                        DCMotor.SpeedCtr(0, 60);
                        break;
                    case 0X70:
                        DCMotor.SpeedCtr(0, 50);
                        break;
                    case 0X30:
                        DCMotor.SpeedCtr(0, 40);
                        break;
                        //偏差小
                    case 0X38:
                        DCMotor.SpeedCtr(10, 40);
                        break;
                        //无需修正
                    case 0X18:
                        DCMotor.SpeedCtr(20, 30);
                        break;
                    case 0X08:
                        DCMotor.SpeedCtr(10, 10);
                        break;
                    case 0x10:
                        DCMotor.SpeedCtr(30, 20);
                        break;
                        //偏差小
                    case 0X1C:
                        DCMotor.SpeedCtr(40, 10);
                        break;
                        //偏差中
                    case 0X0C:
                        DCMotor.SpeedCtr(40, 0);
                        break;
                    case 0X0E:
                        DCMotor.SpeedCtr(50, 0);
                        break;
                    case 0X06:
                        DCMotor.SpeedCtr(60, 0);
                        break;
                        //偏差大
                    case 0X07:
                        DCMotor.SpeedCtr(60, 0);
                        break;
                    case 0X03:
                        DCMotor.SpeedCtr(70, 0);
                        break;
                    case 0X01:
                        DCMotor.SpeedCtr(80, 0);
                        break;
                    default:
                        DCMotor.SpeedCtr(Car_Speed, Car_Speed);
                        break;
                }
                if (Data_OpenMVBuf[5] == 0xff || Data_OpenMVBuf[5] == 0x7F || Data_OpenMVBuf[5] == 0x3F || Data_OpenMVBuf[5] == 0x1F || Data_OpenMVBuf[5] == 0x0F || Data_OpenMVBuf[5] == 0xFE || Data_OpenMVBuf[5] == 0xFC ||
                    Data_OpenMVBuf[5] == 0xF8 || Data_OpenMVBuf[5] == 0xF0)  // 路口
                {
                    OpenMV_Stop();
                    OpenMV_TrackNewtime(10, 1100);
                    OpenMVTrack_Disc_CloseUpNEW();
                    break;
                }
            }
        }
    }
}

AlarmDesk alarmDeskA(0x07);
BarrierGate barrierGateA(0x03);
BusStop busStopA(0x06);
Monitor monitorA(0x04);
Carport carportA(0x0D);
Carport carportB(0x01);
TrafficLight trafficLightA(0x0E);
TrafficLight trafficLightB(0x0F);
TrafficLight trafficLightC(0x13);
TrafficLight trafficLightD(0x14);
StreetLamp streetLampA(0xFF);
K230 k230(0x02);
Car car;