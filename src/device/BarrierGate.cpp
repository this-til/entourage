//
// Created by til on 2025/2/26.
//

#include "BarrierGate.h"


BarrierGate barrierGateA(0x03);

BarrierGate::BarrierGate(uint8_t id) {
    this->id = id;
}

void BarrierGate::setGateControl(bool open) {
    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0x01, static_cast<uint8_t>(open ? 0x01 : 0x02), 0x00, 0x00, 0x00, DATA_FRAME_TAIL};
    Command.Judgment(buf);
    ExtSRAMInterface.ExMem_Write_Bytes(BUS_BASE_ADD, buf, 8);
}

bool BarrierGate::getGateControl(int awaitTimeMs) {
    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0x20, 0x01, 0x00, 0x00, 0x00, DATA_FRAME_TAIL};
    Command.Judgment(buf);  //计算校验和
    ExtSRAMInterface.ExMem_Write_Bytes(BUS_BASE_ADD, buf, 8);

    delay(awaitTimeMs);
    ulong startTime = millis();
    while (ExtSRAMInterface.ExMem_Read(BUS_BASE_ADD) != 0x00) {
        delay(10);
        if (millis() - startTime > awaitTimeMs) {
            return false;
        }
    }

    ExtSRAMInterface.ExMem_Read_Bytes(buf, 8);
    uint8_t checksum = buf[6];    //获取校验和
    Command.Judgment(buf);
    if ((checksum == buf[6]) && (buf[0] == DATA_FRAME_HEADER) && (buf[7] == DATA_FRAME_TAIL)) {
        return true;
    }

    return false;
}

