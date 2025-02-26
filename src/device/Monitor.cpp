//
// Created by til on 2025/2/26.
//

#include "Monitor.h"

Monitor monitorA(0x04);

Monitor::Monitor(int id) {
    this->id = id;
}

void Monitor::setDisplayData(uint8_t pos, uint8_t value[]) {
    uint8_t buf[] = {DATA_FRAME_HEADER, id, pos, 0x00, 0x00, 0x00, 0x00, DATA_FRAME_TAIL};
    buf[3] = value[0] << 8 | value[1];
    buf[4] = value[2] << 8 | value[3];
    buf[5] = value[4] << 8 | value[4];
    Command.Judgment(buf);
    ExtSRAMInterface.ExMem_Write_Bytes(BUS_BASE_ADD, buf, 8);

}

void Monitor::setTimingMode(TimingMode timingMode) {
    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0x03, (uint8_t) timingMode, 0x00, 0x00, 0x00, DATA_FRAME_TAIL};
    Command.Judgment(buf);
    ExtSRAMInterface.ExMem_Write_Bytes(BUS_BASE_ADD, buf, 8);
}

void Monitor::setDistance(uint16_t distance_mm) {
    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0x04, 0x00, (uint8_t) (distance_mm >> 8), (uint8_t) distance_mm, 0x00, DATA_FRAME_TAIL};
    Command.Judgment(buf);
    ExtSRAMInterface.ExMem_Write_Bytes(BUS_BASE_ADD, buf, 8);
}
