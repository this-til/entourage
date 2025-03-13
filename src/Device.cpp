//
// Created by til on 2025/2/27.
//

#include "Device.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wshadow"

DeviceBase* allDeviceBase[256] = {};


void deviceSetup() {
    allDeviceBase[barrierGateA.id] = &barrierGateA;
    allDeviceBase[busStopA.id] = &busStopA;
    allDeviceBase[monitorA.id] = &monitorA;
    allDeviceBase[carportA.id] = &carportA;
    allDeviceBase[carportB.id] = &carportB;
    allDeviceBase[trafficLightA.id] = &trafficLightA;
    allDeviceBase[trafficLightB.id] = &trafficLightB;
    allDeviceBase[trafficLightC.id] = &trafficLightC;
    allDeviceBase[trafficLightD.id] = &trafficLightD;
    allDeviceBase[wirelessChargingA.id] = &wirelessChargingA;
    allDeviceBase[informationDisplayA.id] = &informationDisplayA;
    allDeviceBase[informationDisplayB.id] = &informationDisplayB;
    allDeviceBase[informationDisplayC.id] = &informationDisplayC;
    allDeviceBase[alarmDeskA.id] = &alarmDeskA;
    allDeviceBase[streetLampA.id] = &streetLampA;
    allDeviceBase[stereoscopicDisplayA.id] = &stereoscopicDisplayA;
    allDeviceBase[mainCar.id] = &mainCar;
    allDeviceBase[car.id] = &car;
}

uint8_t MessageBus::computeVerify(const uint8_t* buf, uint8_t len) {
    uint8_t com = 0;
    for (uint8_t i = 2; i < len - 2; i++) {
        com += buf[i];
    }
    return com;
}

void MessageBus::addVerify(uint8_t* buf, uint8_t len) {
    buf[len - 2] = computeVerify(buf, len);
}

void MessageBus::send(uint8_t* buf, uint8_t len, uint16_t bus, bool _addVerify, unsigned long sendCoolingMs) {
    if (millis() - lastSentTime < sendCoolingMs) {
        sleep(sendCoolingMs - (millis() - lastSentTime));
    }
    lastSentTime = millis();

    if (_addVerify) {
        addVerify(buf, len);
    }
#if DE_BUG
    Serial.print("[DEBUG] send:");
    logHex(buf, len);
    Serial.println();
#endif
    ExtSRAMInterface.ExMem_Write_Bytes(bus, buf, len);
}

bool MessageBus::check(const uint8_t* buf, uint8_t len, uint8_t id, const uint8_t* serviceId, uint8_t serviceLen, bool verify) {
    if (buf[0] != DATA_FRAME_HEADER) {
#if DE_BUG
        Serial.print("  inaccuracy header:");
        logHex(buf[0]);
#endif
        return false;
    }

    if (buf[len - 1] != DATA_FRAME_TAIL) {
#if DE_BUG
        Serial.print("  inaccuracy tail:");
        logHex(buf[len - 1]);
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

    if (verify) {
        uint8_t checksum = computeVerify(buf, len);
        if (buf[len - 2] != checksum) {
#if DE_BUG
            Serial.print("  inaccuracy checksum:");
            Serial.print("predict:");
            logHex(checksum);
            Serial.print(",source:");
            logHex(buf[len - 2]);
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

bool MessageBus::check(const uint8_t* buf, uint8_t len, uint8_t id, uint8_t serviceId, bool verify) {
    return check(buf, len, id, &serviceId, 1, verify);
}

bool MessageBus::awaitReturn(uint8_t* buf, uint8_t len, uint8_t id, const uint8_t* serviceId, uint8_t serviceLen, uint16_t bus, bool verify, unsigned long readOutTimeMs) {
    unsigned long startTime = millis();

#if DE_BUG
    Serial.println("[DEBUG] awaitReturn...");
#endif
    while (millis() - startTime < readOutTimeMs) {
        if (ExtSRAMInterface.ExMem_Read(bus) == 0x00) {
            continue;
        }
        ExtSRAMInterface.ExMem_Read_Bytes(bus, buf, len);
#if DE_BUG
        Serial.print("[DEBUG] read:");
        logHex(buf, len);
#endif
        if (check(buf, len, id, serviceId, serviceLen, verify)) {
            return true;
        }
        yield();
    }
#if DE_BUG
    Serial.println("[WARN] outTime...");
#endif
    return false;
}

bool MessageBus::awaitReturn(uint8_t* buf, uint8_t len, uint8_t id, uint8_t serviceId, uint16_t bus, bool verify, unsigned long readOutTimeMs) {
    return awaitReturn(buf, len, id, &serviceId, 1, bus, verify, readOutTimeMs);
}


DeviceBase::DeviceBase(uint8_t id) {
    this->id = id;
}

void DeviceBase::onReceiveZigbeeMessage(uint8_t* buf) {
}

AlarmDesk::AlarmDesk(uint8_t id) : DeviceBase(id) {
}

void AlarmDesk::onReceiveZigbeeMessage(uint8_t* buf) {
    DeviceBase::onReceiveZigbeeMessage(buf);
    UPDATE_VALUE(buf[2] == 0x01, rescuePosition, buf[3])
}

void AlarmDesk::openByInfrared(uint8_t* openCode) {
    Infrare.Transmition(openCode, 6);
}

/***
   * 获取救援位置
   */

bool AlarmDesk::getRescuePosition(uint8_t* rescuePosition) {
    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0x09, 0x00, 0x00, 0x00, 0x00, DATA_FRAME_TAIL};
    messageBus.send(buf, 8);

    DETECTED_VALUE_CHANGE(rescuePosition)
}

/***
                        * debug
 * 设置开启码
 * @param data[] len=6
 */
void AlarmDesk::setOpenCode(uint8_t* data) {
    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0x10, data[5], data[4], data[3], 0x00, DATA_FRAME_TAIL};
    messageBus.send(buf, 8);

    uint8_t buf2[] = {DATA_FRAME_HEADER, id, 0x11, data[2], data[1], data[0], 0x00, DATA_FRAME_TAIL};
    messageBus.send(buf, 8);
}


BarrierGate::BarrierGate(uint8_t id) : DeviceBase(id) {
}

void BarrierGate::setGateControl(bool open) {
    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0x01, static_cast<uint8_t>(open ? 0x01 : 0x02), 0x00, 0x00, 0x00, DATA_FRAME_TAIL};
    messageBus.send(buf, 8);
}

void BarrierGate::setInitialPos(bool isUp) {
    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0x09, static_cast<uint8_t>(isUp ? 0x01 : 0x02), 0x00, 0x00, 0x00, DATA_FRAME_TAIL};
    messageBus.send(buf, 8);
}

void BarrierGate::setLicensePlateData(uint8_t* data) {
    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0x10, data[5], data[4], data[3], 0x00, DATA_FRAME_TAIL};
    messageBus.send(buf, 8);

    uint8_t buf2[] = {DATA_FRAME_HEADER, id, 0x11, data[2], data[1], data[0], 0x00, DATA_FRAME_TAIL};
    messageBus.send(buf2, 8);
}

bool BarrierGate::getGateControl() {
    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0x20, 0x01, 0x00, 0x00, 0x00, DATA_FRAME_TAIL};
    messageBus.send(buf, 8);

    bool* gateControl = nullptr;
    DETECTED_VALUE_CHANGE(gateControl)
}


BusStop::BusStop(uint8_t id) : DeviceBase(id) {
}

void BusStop::onReceiveZigbeeMessage(uint8_t* buf) {
    DeviceBase::onReceiveZigbeeMessage(buf);
    THERE_UPDATE_VALUE(buf[2] == 0x02, yy, buf[3], MM, buf[4], dd, buf[5])
    THERE_UPDATE_VALUE(buf[2] == 0x03, HH, buf[3], mm, buf[4], ss, buf[5])
    TWO_UPDATE_VALUE(buf[2] == 0x04, weather, (Weather) buf[3], temperature, buf[4])
}

void BusStop::broadcastSpeech(uint8_t lid) {
    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0x10, lid, 0x00, 0x00, 0x00, DATA_FRAME_TAIL};
    messageBus.send(buf, 8);
}

void BusStop::broadcastRandomSpeech() {
    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0x20, 0x01, 0x00, 0x00, 0x00, DATA_FRAME_TAIL};
    messageBus.send(buf, 8);
}

void BusStop::setRtcStartDate(uint8_t yy, uint8_t MM, uint8_t dd) {
    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0x30, yy, MM, dd, 0x00, DATA_FRAME_TAIL};
    messageBus.send(buf, 8);
}

void BusStop::setRtcStartTime(uint8_t HH, uint8_t mm, uint8_t ss) {
    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0x40, HH, mm, ss, 0x00, DATA_FRAME_TAIL};
    messageBus.send(buf, 8);
}

bool BusStop::getRtcDate(uint8_t* yy, uint8_t* MM, uint8_t* dd) {
    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0x31, 0x00, 0x00, 0x00, 0x00, DATA_FRAME_TAIL};
    messageBus.send(buf, 8);
    THERE_DETECTED_VALUE_CHANGE(yy, MM, dd)
}

bool BusStop::getRtcTime(uint8_t* HH, uint8_t* mm, uint8_t* ss) {
    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0x41, 0x00, 0x00, 0x00, 0x00, DATA_FRAME_TAIL};
    messageBus.send(buf, 8);
    THERE_DETECTED_VALUE_CHANGE(HH, mm, ss)

}

void BusStop::setWeatherTemperature(Weather weather, uint8_t temperature) {
    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0x42, (uint8_t) weather, temperature, 0x00, 0x00, DATA_FRAME_TAIL};
    messageBus.send(buf, 8);
}

bool BusStop::getWeatherTemperature(Weather* weather, uint8_t* temperature) {
    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0x43, 0x00, 0x00, 0x00, 0x00, DATA_FRAME_TAIL};
    messageBus.send(buf, 8);

    TWO_DETECTED_VALUE_CHANGE(weather, temperature);
    /*bool successful = awaitReturn(buf, 0x04);
    if (successful) {
        *weather = Weather(buf[3]);
        *temperature = buf[4];
    }
    return successful;*/
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
    messageBus.send(buf, 8);

}

void Monitor::setTimingMode(TimingMode timingMode) {
#if DE_BUG
    Serial.print("[DEBUG] setTimingMode:");
    logObj(timingMode);
    Serial.println();
#endif
    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0x03, (uint8_t) timingMode, 0x00, 0x00, 0x00, DATA_FRAME_TAIL};
    messageBus.send(buf, 8);
}

void Monitor::setDistance(uint16_t distance_mm) {
    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0x04, 0x00, (uint8_t) (distance_mm >> 8), (uint8_t) distance_mm, 0x00, DATA_FRAME_TAIL};
    messageBus.send(buf, 8);
}

Carport::Carport(uint8_t id) : DeviceBase(id) {
}

void Carport::onReceiveZigbeeMessage(uint8_t* buf) {
    DeviceBase::onReceiveZigbeeMessage(buf);
    UPDATE_VALUE(buf[2] == 0x03 && buf[3] == 0x01, level, buf[4])
    TWO_UPDATE_VALUE(buf[2] == 0x03 && buf[3] == 0x02, ventral, buf[4] == 0x02, rearSide, buf[5] == 0x02)
}

void Carport::moveToLevel(uint8_t level) {
    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0x01, level, 0x01, 0x00, 0x00, DATA_FRAME_TAIL};
    messageBus.send(buf, 8);
}

bool Carport::getLevel(uint8_t* level) {
#if DE_BUG
    Serial.println("getLevel...");
#endif
    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0x02, 0x01, 0x00, 0x00, 0x00, DATA_FRAME_TAIL};
    messageBus.send(buf, 8);

    DETECTED_VALUE_CHANGE(level)

    /*uint8_t serviceId[] = {0x03, 0x01};
    bool successful = awaitReturn(buf, serviceId, 2);
    if (successful) {
        *level = buf[4];
#if DE_BUG
        Serial.print("level:");
        logHex(*level);
        Serial.println();
#endif
    }
    return successful;*/
}

bool Carport::getInfraredState(bool* ventral, bool* rearSide) {
#if DE_BUG
    Serial.println("getInfraredState...");
#endif
    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0x02, 0x02, 0x00, 0x00, 0x00, DATA_FRAME_TAIL};
    messageBus.send(buf, 8);
    TWO_DETECTED_VALUE_CHANGE(ventral, rearSide)

    /*uint8_t serviceId[] = {0x03, 0x02};
    bool s = awaitReturn(buf, serviceId, 2);
    if (s) {
        *ventral = buf[4] == 0x02;
        *rearSide = buf[5] == 0x02;
#if DE_BUG
        Serial.print("ventral:");
        logBool(*ventral);
        Serial.print("  rearSide:");
        logBool(*rearSide);
        Serial.println();
#endif
    }
    return s;*/
}

TrafficLight::TrafficLight(uint8_t id) : DeviceBase(id) {
}

void TrafficLight::requestToEnterRecognitionMode() {
    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0x01, 0x00, 0x00, 0x00, 0x00, DATA_FRAME_TAIL};
    messageBus.send(buf, 8);
}

void TrafficLight::requestConfirmationOfIdentificationResults(TrafficLightModel trafficLightModel) {
    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0x02, (uint8_t) trafficLightModel, 0x00, 0x00, 0x00, DATA_FRAME_TAIL};
    messageBus.send(buf, 8);
}

StreetLamp::StreetLamp(uint8_t id) : DeviceBase(id) {}

void StreetLamp::setLightSourceIntensity(LightSourceIntensity lightSourceIntensity) {
    uint8_t buf[4] = {0x00, 0xFF, (uint8_t) lightSourceIntensity, (uint8_t) ~lightSourceIntensity};
    Infrare.Transmition(buf, 4);
}

WirelessCharging::WirelessCharging(uint8_t id) : DeviceBase(id) {

}

void WirelessCharging::setOpenWirelessCharging(bool open) {
    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0x01, static_cast<uint8_t>(open ? 0x01 : 0x02), 0x00, 0x00, 0x00, DATA_FRAME_TAIL};
    messageBus.send(buf, 8);
}

void WirelessCharging::openWirelessCharging(uint8_t* openCode) {
    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0x02, openCode[2], openCode[1], openCode[0], 0x00, DATA_FRAME_TAIL};
    messageBus.send(buf, 8);
}

void WirelessCharging::setWirelessChargingOpenCode(uint8_t* openCode) {
    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0x03, openCode[2], openCode[1], openCode[0], 0x00, DATA_FRAME_TAIL};
    messageBus.send(buf, 8);
}

InformationDisplay::InformationDisplay(uint8_t id) : DeviceBase(id) {

}

void InformationDisplay::showSpecifiedPicture(uint8_t id) {
    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0x10, 0x00, id, 0x00, 0x00, DATA_FRAME_TAIL};
    messageBus.send(buf, 8);
}

void InformationDisplay::setThePageTurningMode(PageTurningMode pageTurningMode) {
    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0x10, static_cast<uint8_t>(pageTurningMode), 0x00, 0x00, 0x00, DATA_FRAME_TAIL};
    messageBus.send(buf, 8);
}

void InformationDisplay::showLicensePlate(uint8_t* licensePlate) {
    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0x20, licensePlate[5], licensePlate[4], licensePlate[3], 0x00, DATA_FRAME_TAIL};
    messageBus.send(buf, 8);
    uint8_t buf2[] = {DATA_FRAME_HEADER, id, 0x21, licensePlate[2], licensePlate[1], licensePlate[0], 0x00, DATA_FRAME_TAIL};
    messageBus.send(buf2, 8);
}

void InformationDisplay::setTimingMode(InformationDisplayTimingMode timingMode) {
    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0x30, static_cast<uint8_t>(timingMode), 0x00, 0x00, 0x00, DATA_FRAME_TAIL};
    messageBus.send(buf, 8);
}

void InformationDisplay::showData(uint8_t* data, BaseFormat baseFormat) {
    uint8_t buf[] = {DATA_FRAME_HEADER, id, static_cast<uint8_t>(baseFormat), data[2], data[1], data[0], 0x00, DATA_FRAME_TAIL};
    messageBus.send(buf, 8);
}

void InformationDisplay::showTrafficSigns(TrafficSign TrafficSign) {
    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0x60, static_cast<uint8_t>(TrafficSign), 0x00, 0x00, 0x00, DATA_FRAME_TAIL};
    messageBus.send(buf, 8);
}

ETC::ETC(uint8_t id) : DeviceBase(id) {

}

void ETC::setInitialPos(bool lUp, bool rUp) {
    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0x08, static_cast<uint8_t>(lUp ? 0x01 : 0x02), static_cast<uint8_t>(rUp ? 0x01 : 0x02), 0x00, 0x00, DATA_FRAME_TAIL};
    messageBus.send(buf, 8);
}

StereoscopicDisplay::StereoscopicDisplay(uint8_t id) : DeviceBase(id) {

}

void StereoscopicDisplay::showLicensePlate(uint8_t* licensePlate, uint8_t x, uint8_t y) {
    uint8_t buf[] = {0xFF, 0x20, licensePlate[0], licensePlate[1], licensePlate[2], licensePlate[3]};
    Infrare.Transmition(buf, 6);
    uint8_t buf2[] = {0xFF, 0x10, licensePlate[4], licensePlate[5], x, y};
    Infrare.Transmition(buf2, 6);
}

void StereoscopicDisplay::showDistance(uint8_t value) {
    uint8_t buf[] = {0xFF, 0x11, static_cast<uint8_t>((value / 10) % 10), static_cast<uint8_t>(value % 10), 0x00, 0x00};
    Infrare.Transmition(buf, 6);
}

void StereoscopicDisplay::showGraphText(Graph graph) {
    uint8_t buf[] = {0xFF, 0x12, static_cast<uint8_t>(graph), 0x00, 0x00, 0x00};
    Infrare.Transmition(buf, 6);
}

void StereoscopicDisplay::showColorText(StereoscopicDisplayColor stereoscopicDisplayColor) {
    uint8_t buf[] = {0xFF, 0x13, static_cast<uint8_t>(stereoscopicDisplayColor), 0x00, 0x00, 0x00};
    Infrare.Transmition(buf, 6);
}

void StereoscopicDisplay::showTrafficWarningSigns(TrafficWarningSigns trafficWarningSigns) {
    uint8_t buf[] = {0xFF, 0x14, static_cast<uint8_t>(trafficWarningSigns), 0x00, 0x00, 0x00};
    Infrare.Transmition(buf, 6);
}

void StereoscopicDisplay::showTrafficSign(TrafficSign trafficSign) {
    uint8_t buf[] = {0xFF, 0x15, static_cast<uint8_t>(trafficSign), 0x00, 0x00, 0x00};
    Infrare.Transmition(buf, 6);
}

void StereoscopicDisplay::showDefaultInformation() {
    uint8_t buf[] = {0xFF, 0x16, 0x01, 0x00, 0x00, 0x00};
    Infrare.Transmition(buf, 6);
}

void StereoscopicDisplay::setTextColor(uint8_t r, uint8_t g, uint8_t b) {
    uint8_t buf[] = {0xFF, 0x17, 0x01, r, g, b};
    Infrare.Transmition(buf, 6);
}

void StereoscopicDisplay::showText(const uint16_t* gbk, uint8_t len) {
    uint8_t buf[] = {0xFF, 0x31, 0x00, 0x00, 0x00, 0x00};
    for (int i = 0; i < len; ++i) {
        buf[3] = gbk[i] >> 8;
        buf[2] = gbk[i];
        if (i == len - 1) {
            buf[4] = 0x55;
        }
        Infrare.Transmition(buf, 6);
    }
}

void StereoscopicDisplay::showTextByZigBee(const uint16_t* gbk, uint8_t len) {
    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0x31, 0x00, 0x00, 0x00, 0x00, DATA_FRAME_TAIL};
    for (int i = 0; i < len; ++i) {
        buf[3] = gbk[i] >> 8;
        buf[4] = gbk[i];
        if (i == len - 1) {
            buf[5] = 0x55;
        }
        messageBus.send(buf, 8);
    }
}

double UltrasonicDevice::ranging(int sys, int rangeFrequency, int wait) {
    double distance = 0;
    for (int i = 0; i < rangeFrequency; ++i) {
        distance += Ultrasonic.Ranging(CM);
        sleep(wait);
    }
    return distance / rangeFrequency;
}

void UltrasonicDevice::adjustDistance(uint8_t carSeep, uint8_t targetDistance, double errorMargin, unsigned long controlTime_ms, int wait) {
    ulong startTime = millis();
    while (millis() - startTime >= controlTime_ms) {
        DCMotor.Stop();
        double distance = Ultrasonic.Ranging(CM);;
        double distanceError = distance - targetDistance;
        if (abs(distanceError) > errorMargin) {
            if (distanceError > errorMargin) {
                DCMotor.Go(carSeep);
            } else {
                DCMotor.Back(carSeep);
            }
        }
        sleep(wait);
    }
    DCMotor.Stop();
}

K230::K230(uint8_t id) : DeviceBase(id) {
}

void K230::setTrackModel(uint8_t model) {

#if DE_BUG
    Serial.print("[DEBUG] setCameraSteeringGearAngle:");
    logBin(model);
    Serial.println();
#endif
    this->trackModel = model;

    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0x91, 0x01, model, 0x00, 0x00, DATA_FRAME_TAIL};
    messageBus.send(buf, 8);

    if (model) {
        getTrackFlagBit(nullptr);
    }
}

void K230::setTrackModel(const TrackModel* trackModel, uint8_t len) {
    uint8_t model = 0;
    for (int i = 0; i < len; ++i) {
        model |= trackModel[i];
    }
    setTrackModel(model);
}

uint8_t K230::getTrackModelCache() {
    return trackModel;
}

int8_t K230::getCameraSteeringGearAngleCache() {
    return cameraSteeringGearAngle;
}

void K230::setCameraSteeringGearAngle(int8_t angle) {

#if DE_BUG
    Serial.print("[DEBUG] setCameraSteeringGearAngle:");
    Serial.print(angle);
    Serial.println();
#endif
    cameraSteeringGearAngle = angle;
    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0x91, 0x02, (uint8_t) angle, 0x00, 0x00, DATA_FRAME_TAIL};
    messageBus.send(buf, 8);

    //Command.Judgment(buf);
    //ExtSRAMInterface.ExMem_Write_Bytes(BUS_BASE_ADD, buf, 8);
    //Serial.messageBus.send(buf, 8);
}

bool K230::getTrackFlagBit(uint8_t* result) {
    uint8_t buf[12] = {};
    uint8_t sbuf[2] = {0x091, 0x01};
    bool successful = messageBus.awaitReturn(buf, 12, id, sbuf, 2, 0x6038);

    if (successful) {
        if (result != nullptr) {
            for (int i = 0; i < 6; ++i) {
                result[i] = buf[4 + i];
            }
        }
#if DE_BUG
        Serial.print("[DEBUG] trackFlagBit:");
        logHex(buf + 4, 6);
        Serial.println();
#endif
    }
    return successful;
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
    messageBus.send(buf, 8);

    uint8_t serviceIds[] = {0x92, 0x01, 0x01};
    if (!messageBus.awaitReturn(buf, 8, id, serviceIds, 3, 0x6038)) {
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
    unsigned long startTime = millis();
    while (millis() - startTime < 1000) {
        if (ExtSRAMInterface.ExMem_Read(0x6038) == 0x00) {
            continue;
        }
        ExtSRAMInterface.ExMem_Read_Bytes(0x6038, qrRecognizeBuf, 32);
#if DE_BUG
        Serial.print("read:");
        logHex(qrRecognizeBuf, 32);
        Serial.println();
#endif

        if (
                qrRecognizeBuf[0] != DATA_FRAME_HEADER
                || qrRecognizeBuf[1] != id
                || qrRecognizeBuf[2] != 0x92
                || qrRecognizeBuf[3] != 0x01
                || qrRecognizeBuf[4] != 0x02
                ) {
#if DE_BUG
            Serial.println("verification failure");
#endif
            continue;
        }

        uint8_t qrId = qrRecognizeBuf[5];

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
        qrMessage->qrColor = K210Color(qrRecognizeBuf[6]);

#if DE_BUG
        Serial.print("receive qr color:");
        logHex(K210Color(qrRecognizeBuf[6]));
        Serial.println();
#endif

        uint8_t messageLen = qrRecognizeBuf[7];
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
            qrMessage->message[i] = qrRecognizeBuf[8 + i];
        }

#if DE_BUG
        Serial.print("message:");
        Serial.write(qrRecognizeBuf + 8, messageLen);
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
    messageBus.send(buf, 8);

    uint8_t serviceIds[] = {0x92, 0x03};
    bool successful = messageBus.awaitReturn(buf, 8, id, serviceIds, 2, 0x6038);

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
    messageBus.send(buf, 8);
    uint8_t serviceIds[] = {0x92, 0xFE};
    bool successful = messageBus.awaitReturn(buf, 8, id, serviceIds, 2, 0x6038);
    receiveTrack = false;
    return successful;
}

MainCar::MainCar(uint8_t id) : DeviceBase(id) {
}

void MainCar::onReceiveZigbeeMessage(uint8_t* buf) {
    DeviceBase::onReceiveZigbeeMessage(buf);
}

void Car::onReceiveZigbeeMessage(uint8_t* buf) {
    DeviceBase::onReceiveZigbeeMessage(buf);
#if DE_BUG
    Serial.println("[DEBUG] onReceiveZigbeeMessage from car:");
#endif

    uint8_t _buf[] = {DATA_FRAME_HEADER, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, DATA_FRAME_TAIL};

    switch (buf[2]) {
        case 0x01: //同步全局变量
            mainCar.globalVariable[buf[3]] = buf[4] << 8 | buf[5];
#if DE_BUG
            Serial.print("[DEBUG] synchronousGlobalVariable:");
#endif
            _buf[2] = ~0x01;
            _buf[3] = buf[3];
            _buf[4] = buf[4];
            _buf[5] = buf[5];
            messageBus.send(_buf, 8);
            break;
        case 0xFE:
            mainCar.globalVariableReturnCount++;
#if DE_BUG
            Serial.print("[DEBUG] globalVariableReturnCount:");
            Serial.print(mainCar.globalVariableReturnCount);
            Serial.println();
#endif
            break;
        case 0x02:
            mainCar.taskCompletion[buf[3]] = true;
            break;
        case 0x81: //起始位置 (b车启动) A车任务6（RFID读取） → 发送路径信息 → B车任务8/11
            mainCar.startPos = buf[3];
            break;
    }

}


bool MainCar::synchronousGlobalVariable(uint8_t name, uint16_t value) {
    name = name > 25 ? name - 'a' : name;
    globalVariable[name] = value;
    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0x01, name, (uint8_t) (value >> 8), (uint8_t) (value), 0x00, DATA_FRAME_TAIL};

    uint8_t globalVariableReturnCount = this->globalVariableReturnCount;
    unsigned long startTime = millis();
    while (millis() - startTime < 5000) {
        messageBus.send(buf, 8);
        if (globalVariableReturnCount != this->globalVariableReturnCount) {
#if DE_BUG
            Serial.println("[DEBUG] synchronousGlobalVariable successful...");
#endif
            return true;
        }
    }
#if DE_BUG
    Serial.println("[DEBUG] synchronousGlobalVariable outTime...");
#endif
    return false;
}


uint16_t MainCar::getGlobalVariable(uint8_t name) {
    name = name > 25 ? name - 'a' : name;
    return globalVariable[name];
}


void MainCar::synchronizationTaskCompletion(uint8_t taskId) {
    taskCompletion[taskId] = true;
    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0x01, taskId, 0x00, 0x00, 0x00, DATA_FRAME_TAIL};
    messageBus.send(buf, 8);
}

void MainCar::sendQr1LicensePlateNumber(uint8_t* data) {
    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0xF1, data[5], data[4], data[3], 0x00, DATA_FRAME_TAIL};
    messageBus.send(buf, 8);
    uint8_t buf2[] = {DATA_FRAME_HEADER, id, 0xF2, data[2], data[1], data[0], 0x00, DATA_FRAME_TAIL};
    messageBus.send(buf2, 8);
}

void MainCar::sendQr2CalculationResult(uint8_t result) {
    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0xF3, result, 0x00, 0x00, 0x00, DATA_FRAME_TAIL};
    messageBus.send(buf, 8);
}

void MainCar::sendCarportALevel(uint8_t level) {
    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0xF4, level, 0x00, 0x00, 0x00, DATA_FRAME_TAIL};
    messageBus.send(buf, 8);
}

Car::Car(uint8_t id) : DeviceBase(id) {
    straightLineSpeed = 20;
    turnLeftSpeed = 40;
    turnRightSpeed = 40;

    straightLineKpSpeed = 60;
    trimKpSpeed = 40;

    outTime_ms = 20000;
    trimOutTime_ms = 1000;

    trackRowResultHigh.flagBitArray = flagBitArray;
    trackRowResult.flagBitArray = flagBitArray + 2;
    trackRowResultLow.flagBitArray = flagBitArray + 4;
}

void Car::carSleep(uint16_t time) {
    unsigned long startTime = millis();

    while (millis() - startTime < time) {
        acceptTrackFlag();
    }
}

void Car::clearCodeDisc() {
    uint8_t buf[] = {DATA_FRAME_HEADER, 0x02, 0x07, 0x00, 0x00, 0x00, 0x00, DATA_FRAME_TAIL};

    {
        uint32_t com = 0;
        for (uint8_t i = 2; i < 8 - 2; i++) {
            com += buf[i];
        }
        buf[8 - 2] = static_cast<int8_t>(com % 256);
    }

    ExtSRAMInterface.ExMem_Write_Bytes(0x6008, buf, 8);
#if DE_BUG
    Serial.println("[DEBUG] clearCodeDisc.");
#endif
}

uint16_t Car::getCodeDisc() {
    uint16_t code = ExtSRAMInterface.ExMem_Read(0x6003) << 8 | ExtSRAMInterface.ExMem_Read(0x6002);
#if DE_BUG
    /*Serial.print("[DEBUG] getCodeDisc:");
    Serial.print(code);
    Serial.println();*/
#endif
    return code;
}

uint16_t Car::getTrackLamp() {
    uint16_t code = ExtSRAMInterface.ExMem_Read(0x6001) << 8 | ExtSRAMInterface.ExMem_Read(0x6000);
#if DE_BUG
    Serial.print("[DEBUG] getTrackLamp:");
    Serial.print(code);
    Serial.println();
#endif
    return code;
}

void Car::turnLeft(bool trimCar) {
#if DE_BUG
    Serial.println("[DEBUG] in turnLeft...");
#endif
    DCMotor.SpeedCtr(-40, 40);
    carSleep(1500);
    DCMotor.Stop();
    if (trimCar) {
        this->trimCar();
    }
#if DE_BUG
    Serial.println("[DEBUG] end turnLeft...");
#endif
}

void Car::turnRight(bool trimCar) {
#if DE_BUG
    Serial.println("[DEBUG] in turnRight...");
#endif
    DCMotor.SpeedCtr(40, -40);
    carSleep(1500);
    DCMotor.Stop();
    if (trimCar) {
        this->trimCar();
    }
#if DE_BUG
    Serial.println("[DEBUG] end turnRight...");
#endif
}

void Car::trackTurnLeft(bool trimCar) {

#if DE_BUG
    Serial.println("[DEBUG] in trackTurnLeft...");
#endif
    DCMotor.SpeedCtr(-turnLeftSpeed, turnLeftSpeed);
    rotationProcess();
    DCMotor.Stop();
    if (trimCar) {
        this->trimCar();
    }
#if DE_BUG
    Serial.println("[DEBUG] end trackTurnLeft...");
#endif
}

void Car::trackTurnRight(bool trimCar) {

#if DE_BUG
    Serial.println("[DEBUG] in trackTurnRight...");
#endif
    DCMotor.SpeedCtr(turnRightSpeed, -turnRightSpeed);
    rotationProcess();
    DCMotor.Stop();
    if (trimCar) {
        this->trimCar();
    }
#if DE_BUG
    Serial.println("[DEBUG] end trackTurnRight...");
#endif
}

void Car::rotationProcess() {
    //sleep(1500);

    unsigned long startTime = millis();

    while (millis() - startTime < outTime_ms) {
        acceptTrackFlag();
        if (!trackRowResult.bitCount) {
            break;
        }
    }
    while (millis() - startTime < outTime_ms) {
        acceptTrackFlag();
        if (trackRowResult.centerBitCount >= 3) {
            break;
        }
    }

    DCMotor.Stop();
}

void Car::waitCodeDisc(int16_t distance) {
    uint16_t old = getCodeDisc();
    while (true) {
        uint16_t current = getCodeDisc();
        auto delta = static_cast<int16_t>(current - old);
        if ((distance > 0 && delta >= distance) || (distance < 0 && delta <= distance)) {
            break;
        }
        yield();
    }
}

void Car::trackAdvanceToNextJunction() {


    unsigned long startTime = millis();

    DCMotor.SpeedCtr(straightLineSpeed, straightLineSpeed);

    while (millis() - startTime < outTime_ms) {
        acceptTrackFlag();

        if ((trackRowResult.edgeBitCount >= 6 || trackRowResult.bitCount >= 12)) {

#if DE_BUG
            Serial.println("[DEBUG] meet a junction...");
#endif

            advance(350);
            trimCar();
            break;

        }

        if (inRand(trackRowResult.offset, -0.2, 0.2)) {

            int16_t lSpeed = straightLineSpeed;
            int16_t rSpeed = straightLineSpeed;

            lSpeed += ((int16_t) (trackRowResult.offset * straightLineKpSpeed));
            rSpeed -= ((int16_t) (trackRowResult.offset * straightLineKpSpeed));

            DCMotor.SpeedCtr(
                    lSpeed,
                    rSpeed
            );
        } else {
            trimCar();
        }

    }
    DCMotor.Stop();

}

void Car::overspecificRelief() {
    unsigned long startTime = millis();

    while (millis() - startTime < outTime_ms) {
        acceptTrackFlag();
        if (trackRowResult.flagBit == 0) {
            break;
        }
        advance(30);
        trimCar();
        sleep(100);
    }

    recoil(50);

    //advance(200);



    /* while (millis() - startTime < outTime_ms) {
         acceptTrackFlag();

         if (trackRowResult.flagBit == 0) {
         }

         trackRowResult.centerBitCount = ~trackRowResult.centerBitCount;
         uint8_t buf[2] = {};
         lonelinessExclusion(flagBitArray, 2, buf);
         centralPoint(buf, 2, nullptr, &trackRowResult.offset);

         if (inRand(trackRowResult.offset, -0.2, 0.2)) {

             int16_t lSpeed = straightLineSpeed;
             int16_t rSpeed = straightLineSpeed;

             lSpeed += ((int16_t) (trackRowResult.offset * straightLineKpSpeed));
             rSpeed -= ((int16_t) (trackRowResult.offset * straightLineKpSpeed));

             DCMotor.SpeedCtr(
                     lSpeed,
                     rSpeed
             );


         } else {
             trimCar();
         }

     }
 */

    DCMotor.Stop();
}

void Car::advanceToNextJunction() {
    unsigned long startTime = millis();

    DCMotor.SpeedCtr(straightLineSpeed, straightLineSpeed);
    while (millis() - startTime < outTime_ms) {
        acceptTrackFlag();
        if ((trackRowResult.edgeBitCount >= 6 || trackRowResult.bitCount >= 12)) {
            DCMotor.Stop();
            break;
        }
    }

}

void Car::recoilToNextJunction() {
    unsigned long startTime = millis();

    DCMotor.SpeedCtr(-straightLineSpeed, -straightLineSpeed);

    while (millis() - startTime < outTime_ms) {
        acceptTrackFlag();

        if ((trackRowResult.edgeBitCount >= 6 || trackRowResult.bitCount >= 12)) {
            DCMotor.Stop();
            break;
        }

    }
}

void Car::advance(uint16_t distance) {
    DCMotor.SpeedCtr(
            straightLineSpeed,
            straightLineSpeed
    );
    waitCodeDisc((int16_t) distance);
    DCMotor.Stop();
}


void Car::recoil(uint16_t distance) {
    DCMotor.SpeedCtr(
            -straightLineSpeed,
            -straightLineSpeed
    );
    waitCodeDisc(-(int16_t) distance);
    DCMotor.Stop();
}


void Car::mobileCorrection(uint16_t step) {
    unsigned long startTime = millis();
    uint8_t positiveAttitude = 0;
    bool advanceTag = true;
    while (millis() - startTime < outTime_ms) {
        if (advanceTag) {
            advance(step);
        } else {
            recoil(step);
        }
        advanceTag = !advanceTag;

        carSleep(100);
        acceptTrackFlag();
        bool inRand = inRand(trackRowResult.offset, -0.08, 0.08);
        trimCar();
        if (inRand) {
            positiveAttitude++;
            if (positiveAttitude >= 3) {
                break;
            }
            continue;
        }

        positiveAttitude = 0;

    }
}

void Car::advanceCorrection(uint16_t step, uint8_t maximumFrequency) {
    unsigned long startTime = millis();
    uint8_t positiveAttitude = 0;
    uint8_t advanceTag = 0;
    while (millis() - startTime < outTime_ms) {

        carSleep(50);
        acceptTrackFlag();
        bool inRand = inRand(trackRowResult.offset, -0.08, 0.08);
        trimCar();
        if (inRand) {
            positiveAttitude++;
            if (positiveAttitude >= 3) {
                break;
            } else {
                positiveAttitude = 0;
            }
        }

        advanceTag++;
        if (advanceTag <= maximumFrequency) {
            advance(step);
        } else {
            break;
        }

    }
}

void Car::trimCar() {
    trimCar(&trackRowResult, 0);
}


void Car::trimCar(TrackRowResult* trackRowResult, float offset) {
#if DE_BUG
    Serial.println("[DEBUG] in trimCar...");
#endif

    unsigned long startTime = millis();
    while (millis() - startTime < trimOutTime_ms) {
        acceptTrackFlag();

        float trimOffset = trackRowResult->offset + offset;
        if (trimOffset == 0) {
            break;
        }

        auto time = (int16_t) (430.0f * trimOffset);
        time = time < 0 ? -time : time;
        time = time > 100 ? 100 : time;

#if DE_BUG
        Serial.print("[DEBUG] offset:");
        Serial.print(trackRowResult->offset);
        Serial.print("  sleep:");
        Serial.print(time);
        Serial.println();
#endif


        DCMotor.SpeedCtr(
                ((int16_t) (tristate(trimOffset, -20, 0, 20))),
                ((int16_t) (tristate(trimOffset, 20, 0, -20)))
        );
        sleep(time);
        DCMotor.Stop();
    }

    DCMotor.Stop();

#if DE_BUG
    Serial.println("[DEBUG] end trimCar...");
#endif
}


/***
 * 矫正车身，与寻迹线平行（增强版）
 * 策略：当高低寻迹线偏移量相等且趋近零时，认为车身已平行
 */
void Car::rightCar() {
    uint8_t trackModel = k230.getTrackModelCache();
    int8_t angle = k230.getCameraSteeringGearAngleCache();

    k230.setCameraSteeringGearAngle(-55);
    k230.setTrackModel(TRACK_LOW | TRACK_HIGH);
    carSleep(200); // 等待摄像头稳定

    unsigned long startTime = millis();
    while (millis() - startTime < trimOutTime_ms) {
        acceptTrackFlag();

        float offsetLow = trackRowResultLow.offset;
        float offsetHigh = trackRowResultHigh.offset;

        if (abs(offsetHigh - offsetLow) == 0) {
            break;
        }

        float finallyOffset = offsetHigh - offsetLow;

        trimCar(&trackRowResultHigh, finallyOffset);

        DCMotor.Stop();

    }

    k230.setCameraSteeringGearAngle(angle);
    k230.setTrackModel(trackModel);
    carSleep(200); // 恢复原始状态的稳定时间
}

/*const float KP_ANGLE = 40.0; // 方向误差比例系数（角度调整）
const float KP_POSITION = 20.0; // 位置误差比例系数（中线调整）


*//***
 * 微调车姿态
 *//*
void Car::trimCar() {

#if DE_BUG
    Serial.println("[DEBUG] in trimCar...");
#endif

    unsigned long startTime = millis();

    while (millis() - startTime < trimOutTime_ms) {
        acceptTrackFlag();

        float angleError = highOffset - lowOffset;
        float positionError = (highOffset + lowOffset) / 2.0f;
        float adjustment = (KP_ANGLE * angleError) + (KP_POSITION * positionError);

        if (fabs((double) angleError) < 0.1 && fabs((double) positionError) < 0.1) {
            break;
        }

        DCMotor.SpeedCtr((int16_t) adjustment, (int16_t) -adjustment);
    }

    DCMotor.Stop();

#if DE_BUG
    Serial.println("[DEBUG] end trimCar...");
#endif
}*/


bool Car::acceptTrackFlag() {
    bool successful = k230.getTrackFlagBit(flagBitArray);

    if (successful) {

        uint8_t tackModel = k230.getTrackModelCache();

        if (tackModel & TRACK_HIGH) {
            acceptTrackRowFlag(&trackRowResultHigh);
        }
        if (tackModel & TRACK_MIDDLE) {
            acceptTrackRowFlag(&trackRowResult);
        }
        if (tackModel & TRACK_LOW) {
            acceptTrackRowFlag(&trackRowResultLow);
        }
    }

    return successful;
}

void Car::acceptTrackRowFlag(TrackRowResult* trackRowResult) {
    trackRowResult->flagBit = trackRowResult->flagBitArray[0] << 8 | trackRowResult->flagBitArray[1];
    centralPoint(trackRowResult->flagBitArray, 2, nullptr, &trackRowResult->offset);
    trackRowResult->bitCount = countBits(trackRowResult->flagBitArray, 2);
    trackRowResult->centerBitCount = countBits(trackRowResult->flagBitArray, 2, 4, 12);
    trackRowResult->edgeBitCount = trackRowResult->bitCount - trackRowResult->centerBitCount;
}

void Car::ledShine(int number, int wait) {
    for (int i = 0; i < number; ++i) {
        carLight.LeftTurnOn();
        carLight.RightTurnOn();
        delay(wait);
        carLight.LeftTurnOff();
        carLight.RightTurnOff();
    }
}


#pragma clang diagnostic pop

MessageBus messageBus;

BarrierGate barrierGateA(0x03);
BusStop busStopA(0x06);
Monitor monitorA(0x04);
Carport carportA(0x0D);
Carport carportB(0x01);
TrafficLight trafficLightA(0x0E);
TrafficLight trafficLightB(0x0F);
TrafficLight trafficLightC(0x13);
TrafficLight trafficLightD(0x14);
WirelessCharging wirelessChargingA(0x0A);
InformationDisplay informationDisplayA(0x0B);
InformationDisplay informationDisplayB(0x08);
InformationDisplay informationDisplayC(0x12);

AlarmDesk alarmDeskA(0x07);
StreetLamp streetLampA(0xFF);
StereoscopicDisplay stereoscopicDisplayA(0x11);

UltrasonicDevice ultrasonicDevice;
K230 k230(0x02);
Car car(0x02);
MainCar mainCar(0x01);
