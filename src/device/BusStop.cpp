//
// Created by til on 2025/2/26.
//

#include "BusStop.h"

BusStop busStopA(0x06);

BusStop::BusStop(uint8_t id) {
    this->id = id;
}

void BusStop::broadcastSpeech(uint8_t lid) {
    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0x10, lid, 0x00, 0x00, 0x00, DATA_FRAME_TAIL};
    Command.Judgment(buf);
    ExtSRAMInterface.ExMem_Write_Bytes(BUS_BASE_ADD, buf, 8);
}

void BusStop::broadcastRandomSpeech() {
    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0x20, 0x01, 0x00, 0x00, 0x00, DATA_FRAME_TAIL};
    Command.Judgment(buf);
    ExtSRAMInterface.ExMem_Write_Bytes(BUS_BASE_ADD, buf, 8);
}

void BusStop::setRtcStartDate(uint8_t yy, uint8_t MM, uint8_t dd) {
    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0x30, yy, MM, dd, 0x00, DATA_FRAME_TAIL};
    Command.Judgment(buf);
    ExtSRAMInterface.ExMem_Write_Bytes(BUS_BASE_ADD, buf, 8);
}

void BusStop::setRtcStartTime(uint8_t HH, uint8_t mm, uint8_t ss) {
    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0x40, HH, mm, ss, 0x00, DATA_FRAME_TAIL};
    Command.Judgment(buf);
    ExtSRAMInterface.ExMem_Write_Bytes(BUS_BASE_ADD, buf, 8);
}

void BusStop::getRtcDate(uint8_t* yy, uint8_t* MM, uint8_t* dd) {
    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0x31, 0x00, 0x00, 0x00, 0x00, DATA_FRAME_TAIL};
    Command.Judgment(buf);
    ExtSRAMInterface.ExMem_Write_Bytes(BUS_BASE_ADD, buf, 8);

    ExtSRAMInterface.ExMem_Read_Bytes(buf, 8);
    uint8_t checksum = buf[6];
    Command.Judgment(buf);
    if ((checksum == buf[6]) && (buf[0] == DATA_FRAME_HEADER) && (buf[7] == DATA_FRAME_TAIL)) {
        *yy = buf[3];
        *MM = buf[4];
        *dd = buf[5];
    }

}

void BusStop::getRtcTime(uint8_t* HH, uint8_t* mm, uint8_t* ss) {
    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0x41, 0x00, 0x00, 0x00, 0x00, DATA_FRAME_TAIL};
    Command.Judgment(buf);
    ExtSRAMInterface.ExMem_Write_Bytes(BUS_BASE_ADD, buf, 8);

    ExtSRAMInterface.ExMem_Read_Bytes(buf, 8);
    uint8_t checksum = buf[6];
    Command.Judgment(buf);
    if ((checksum == buf[6]) && (buf[0] == DATA_FRAME_HEADER) && (buf[7] == DATA_FRAME_TAIL)) {
        *HH = buf[3];
        *mm = buf[4];
        *ss = buf[5];
    }
}

void BusStop::setWeatherTemperature(Weather weather, uint8_t temperature) {
    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0x42, (uint8_t) weather, temperature, 0x00, 0x00, DATA_FRAME_TAIL};
    Command.Judgment(buf);
    ExtSRAMInterface.ExMem_Write_Bytes(BUS_BASE_ADD, buf, 8);
}

void BusStop::getWeatherTemperature(Weather* weather, uint8_t* temperature) {
    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0x43, 0x00, 0x00, 0x00, 0x00, DATA_FRAME_TAIL};
    Command.Judgment(buf);
    ExtSRAMInterface.ExMem_Write_Bytes(BUS_BASE_ADD, buf, 8);

    ExtSRAMInterface.ExMem_Read_Bytes(buf, 8);
    uint8_t checksum = buf[6];
    Command.Judgment(buf);
    if ((checksum == buf[6]) && (buf[0] == DATA_FRAME_HEADER) && (buf[7] == DATA_FRAME_TAIL)) {
        *weather = Weather(buf[3]);
        *temperature = buf[4];
    }
}