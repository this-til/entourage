//
// Created by til on 2025/2/27.
//

#include "Device.h"

DeviceBase::DeviceBase(uint8_t id) {
    this->id = id;

    this->writeBus = 0x6008;
    this->writeLen = 8;
    this->writeVerify = true;

    this->readBus = 0x6100;
    this->readLen = 8;
    this->readVerify = true;
    this->readTailIntegrity = true;
    this->readOutTime_ms = 500;
}

void DeviceBase::write(uint8_t* buf) {
    if (writeVerify) {
        uint32_t com = 0;
        for (uint8_t i = 2; i < writeLen - 2; i++) {
            com += buf[i];
        }
        buf[readLen - 2] = uint8_t(com % 256);
    }
    ExtSRAMInterface.ExMem_Write_Bytes(writeBus, buf, writeLen);
}

bool DeviceBase::verificationReturn(const uint8_t* buf, const uint8_t* serviceId, uint8_t serviceLen) {
    if (buf[0] != DATA_FRAME_HEADER) {
        return false;
    }
    if (readTailIntegrity && buf[readLen - 1] != DATA_FRAME_TAIL) {
        return false;
    }
    if (buf[1] != id) {
        return false;
    }
    for (int i = 0; i < serviceLen; ++i) {
        if (buf[2 + i] != serviceId[i]) {
            return false;
        }
    }

    if (readVerify) {
        uint32_t com = 0;
        for (uint8_t i = 1; i < readLen - 2; i++) {
            com += buf[i];
        }
        if (buf[readLen - 2] != uint8_t(com % 256)) {
            return false;
        }
    }

    return true;
}


bool DeviceBase::awaitReturn(uint8_t* buf, const uint8_t* serviceId, uint8_t serviceLen) {
    unsigned long startTime = millis();
    while (millis() - startTime < readOutTime_ms) {
        if (ExtSRAMInterface.ExMem_Read(readBus) == 0x00) {
            continue;
        }
        ExtSRAMInterface.ExMem_Read_Bytes(readBus, buf, readLen);
        if (verificationReturn(buf, serviceId, serviceLen)) {
            return true;
        }
    }
    return false;
}

bool DeviceBase::verificationReturn(const uint8_t* buf, uint8_t serviceId) {
    return verificationReturn(buf, &serviceId, 1);
}

bool DeviceBase::awaitReturn(uint8_t* buf, uint8_t serviceId) {
    return awaitReturn(buf, &serviceId, 1);
}


AlarmDesk::AlarmDesk(uint8_t id) : DeviceBase(id) {
}

void AlarmDesk::openByInfrared() {
    //TODO 红外通信
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
    write(buf);

    uint8_t buf2[] = {DATA_FRAME_HEADER, id, 0x11, data[2], data[1], data[0], 0x00, DATA_FRAME_TAIL};
    write(buf2);
}


BarrierGate::BarrierGate(uint8_t id) : DeviceBase(id) {
}

void BarrierGate::setGateControl(bool open) {
    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0x01, static_cast<uint8_t>(open ? 0x01 : 0x02), 0x00, 0x00, 0x00, DATA_FRAME_TAIL};
    write(buf);
}

bool BarrierGate::getGateControl() {
    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0x20, 0x01, 0x00, 0x00, 0x00, DATA_FRAME_TAIL};
    write(buf);

    return awaitReturn(buf, 0x01);;
}

BusStop::BusStop(uint8_t id) : DeviceBase(id) {
}

void BusStop::broadcastSpeech(uint8_t lid) {
    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0x10, lid, 0x00, 0x00, 0x00, DATA_FRAME_TAIL};
    write(buf);
}

void BusStop::broadcastRandomSpeech() {
    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0x20, 0x01, 0x00, 0x00, 0x00, DATA_FRAME_TAIL};
    write(buf);
}

void BusStop::setRtcStartDate(uint8_t yy, uint8_t MM, uint8_t dd) {
    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0x30, yy, MM, dd, 0x00, DATA_FRAME_TAIL};
    write(buf);
}

void BusStop::setRtcStartTime(uint8_t HH, uint8_t mm, uint8_t ss) {
    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0x40, HH, mm, ss, 0x00, DATA_FRAME_TAIL};
    write(buf);
}

void BusStop::getRtcDate(uint8_t* yy, uint8_t* MM, uint8_t* dd) {
    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0x31, 0x00, 0x00, 0x00, 0x00, DATA_FRAME_TAIL};
    write(buf);

    if (awaitReturn(buf, 0x02)) {
        *yy = buf[3];
        *MM = buf[4];
        *dd = buf[5];
    }

}

void BusStop::getRtcTime(uint8_t* HH, uint8_t* mm, uint8_t* ss) {
    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0x41, 0x00, 0x00, 0x00, 0x00, DATA_FRAME_TAIL};
    write(buf);

    if (awaitReturn(buf, 0x03)) {
        *HH = buf[3];
        *mm = buf[4];
        *ss = buf[5];
    }

}

void BusStop::setWeatherTemperature(Weather weather, uint8_t temperature) {
    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0x42, (uint8_t) weather, temperature, 0x00, 0x00, DATA_FRAME_TAIL};
    write(buf);
}

void BusStop::getWeatherTemperature(Weather* weather, uint8_t* temperature) {
    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0x43, 0x00, 0x00, 0x00, 0x00, DATA_FRAME_TAIL};
    write(buf);

    if (awaitReturn(buf, 0x04)) {
        *weather = Weather(buf[3]);
        *temperature = buf[4];
    }

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
    write(buf);

}

void Monitor::setTimingMode(TimingMode timingMode) {
    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0x03, (uint8_t) timingMode, 0x00, 0x00, 0x00, DATA_FRAME_TAIL};
    write(buf);
}

void Monitor::setDistance(uint16_t distance_mm) {
    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0x04, 0x00, (uint8_t) (distance_mm >> 8), (uint8_t) distance_mm, 0x00, DATA_FRAME_TAIL};
    write(buf);
}

Carport::Carport(uint8_t id) : DeviceBase(id) {
}

void Carport::moveToLevel(uint8_t level) {
    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0x01, level, 0x01, 0x00, 0x00, DATA_FRAME_TAIL};
    write(buf);
}

uint8_t Carport::getLevel() {
    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0x02, 0x01, 0x00, 0x00, 0x00, DATA_FRAME_TAIL};
    write(buf);

    if (awaitReturn(buf, 0x01)) {
        return buf[4];
    }
    return 0;
}

void Carport::getInfraredState(bool* ventral, bool* rearSide) {
    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0x02, 0x02, 0x00, 0x00, 0x00, DATA_FRAME_TAIL};
    write(buf);

    if (awaitReturn(buf, 0x02)) {
        *ventral = buf[4] == 0x01;
        *rearSide = buf[5] == 0x02;
    }
}

TrafficLight::TrafficLight(uint8_t id) : DeviceBase(id) {
}

void TrafficLight::requestToEnterRecognitionMode() {
    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0x01, 0x00, 0x00, 0x00, 0x00, DATA_FRAME_TAIL};
    write(buf);
}

void TrafficLight::requestConfirmationOfIdentificationResults(TrafficLightModel trafficLightModel) {
    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0x02, (uint8_t) trafficLightModel, 0x00, 0x00, 0x00, DATA_FRAME_TAIL};
    write(buf);
}

StreetLamp::StreetLamp(uint8_t id) : DeviceBase(id) {}

K230::K230(uint8_t id) : DeviceBase(id) {
    readBus = 0x6038;
    readVerify = false;
}

void K230::setCameraSteeringGearAngle(int8_t angle) {
    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0x91, 0x03, (uint8_t) angle, 0x00, 0x00, DATA_FRAME_TAIL};
    write(buf);
    //Command.Judgment(buf);
    //ExtSRAMInterface.ExMem_Write_Bytes(BUS_BASE_ADD, buf, 8);
    //Serial.write(buf, 8);
}


void K230::setTrackModel(bool open) {
    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0x91, static_cast<uint8_t>(open ? 0x01 : 0x02), 0x00, 0x00, 0x00, DATA_FRAME_TAIL};
    write(buf);
}

void K230::qrRecognize(uint8_t* count, QrMessage* qrMessageArray, uint8_t maxLen) {
    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0x92, 0x01, 0x00, 0x00, 0x00, DATA_FRAME_TAIL};
    write(buf);

    uint8_t serviceIds[] = {0x92, 0x01, 0x01};
    if (!awaitReturn(buf, serviceIds, 3)) {
        *count = 0;
        return;
    }

    uint8_t s_count = buf[5];

    s_count = s_count > maxLen
              ? maxLen
              : s_count < 0
                ? 0
                : s_count;
    *count = s_count;

    uint8_t hitCount = 0;

    uint8_t bufLong[64] = {};

    unsigned long startTime = millis();
    while (millis() - startTime < readOutTime_ms) {
        if (ExtSRAMInterface.ExMem_Read(BUS_BASE_ADD) == 0x00) {
            continue;
        }
        ExtSRAMInterface.ExMem_Read_Bytes(bufLong, 64);
        /*if (!verificationReturn(bufLong, 0x02, 64, false, false)) {
            continue;
        }*/

        if (buf[0] != DATA_FRAME_HEADER) {
            continue;
        }
        if (buf[1] != id) {
            continue;
        }
        if (buf[2] != 0x02) {
            continue;
        }
        if (buf[3] != 0x92) {
            continue;
        }
        if (buf[4] != 0x02) {
            continue;
        }

        uint8_t qrId = bufLong[5];

        if (qrId >= maxLen) {
            continue;
        }

        QrMessage* qrMessage = qrMessageArray + qrId;

        qrMessage->efficient = true;
        qrMessage->qrColor = K210Color(bufLong[6]);

        uint8_t messageLen = bufLong[7];
        messageLen = messageLen > qrMessage->messageMaxLen
                     ? qrMessage->messageMaxLen
                     : messageLen > 0
                       ? 0
                       : messageLen;

        qrMessage->messageLen = messageLen;

        for (int i = 0; i < messageLen; ++i) {
            qrMessage->message[i] = bufLong[8 + i];
        }

        hitCount++;
        if (hitCount >= s_count) {
            break;
        }
    }

}

K210Color K230::trafficLightRecognize() {
    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0x92, 0x03, 0x00, 0x00, 0x00, DATA_FRAME_TAIL};
    write(buf);

    uint8_t serviceIds[] = {0x92, 0x03};
    if (awaitReturn(buf, serviceIds, 2)) {
        return K210Color(buf[3]);
    }

    return K_NONE;
}

bool K230::ping() {
    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0x92, 0xFE, 0x00, 0x00, 0x00, DATA_FRAME_TAIL};
    write(buf);
    uint8_t serviceIds[] = {0x92, 0xFE};
    return awaitReturn(buf, serviceIds, 2);
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
