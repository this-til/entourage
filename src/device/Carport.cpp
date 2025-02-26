#include "Carport.h"

//=================以下为车库封装============================



Carport carportA(0x0D);
Carport carportB(0x01);

Carport::Carport(uint8_t id) {
    this->id = id;
}

void Carport::moveToLevel(uint8_t level) {
    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0x01, level, 0x01, 0x00, 0x00, DATA_FRAME_TAIL};
    Command.Judgment(buf);  //计算校验和
    ExtSRAMInterface.ExMem_Write_Bytes(BUS_BASE_ADD, buf, 8);
}


uint8_t Carport::getLevel() {
    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0x02, 0x01, 0x00, 0x00, 0x00, DATA_FRAME_TAIL};
    Command.Judgment(buf);  //计算校验和
    ExtSRAMInterface.ExMem_Write_Bytes(BUS_BASE_ADD, buf, 8);

    ExtSRAMInterface.ExMem_Read_Bytes(buf, 8);
    uint8_t checksum = buf[6];    //获取校验和
    Command.Judgment(buf);
    if ((checksum == buf[6]) && (buf[0] == 0x55) && (buf[7] == 0xBB)) {
        return buf[4];
    }
    return 0;
}

void Carport::getInfraredState(bool* ventral, bool* rearSide) {
    uint8_t buf[] = {DATA_FRAME_HEADER, id, 0x02, 0x02, 0x00, 0x00, 0x00, DATA_FRAME_TAIL};
    Command.Judgment(buf);
    ExtSRAMInterface.ExMem_Write_Bytes(BUS_BASE_ADD, buf, 8);

    ExtSRAMInterface.ExMem_Read_Bytes(buf, 8);
    uint8_t checksum = buf[6];
    Command.Judgment(buf);
    if ((checksum == buf[6]) && (buf[0] == 0x55) && (buf[7] == 0xBB)) {
        *ventral = buf[4] == 0x01;
        *rearSide = buf[5] == 0x02;
        return;
    }
}


void Road_Gate_TestA(const char* data) {
    uint8_t road_bufa[] = {DATA_FRAME_HEADER, 0x03, 0x10, 0x01, 0x00, 0x00, 0x00, DATA_FRAME_TAIL};
    uint8_t road_bufb[] = {DATA_FRAME_HEADER, 0x03, 0x11, 0x01, 0x00, 0x00, 0x00, DATA_FRAME_TAIL};
    road_bufa[3] = data[0];
    road_bufa[4] = data[1];
    road_bufa[5] = data[2];
    road_bufb[3] = data[3];
    road_bufb[4] = data[4];
    road_bufb[5] = data[5];
    Command.Judgment(road_bufa);  //计算校验和
    Command.Judgment(road_bufb);  //计算校验和

    delay(1000);
    ExtSRAMInterface.ExMem_Write_Bytes(BUS_BASE_ADD, road_bufa, 8);
    delay(1000);
    ExtSRAMInterface.ExMem_Write_Bytes(BUS_BASE_ADD, road_bufb, 8);
}




