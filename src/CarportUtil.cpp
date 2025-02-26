#include "CarportUtil.h"

//=================以下为车库封装============================


#define MAXIMUM_RETRY 5
#define WAITING_INTERVAL_MS 100


void carport_moveToLevel(uint8_t level) {
    uint8_t buf[] = {0x55, 0x0D, 0x01, level, 0x01, 0x00, 0x00, 0xBB};
    Command.Judgment(buf);  //计算校验和
    ExtSRAMInterface.ExMem_Write_Bytes(0x6008, buf, 8);
}


uint8_t carport_getLevel() {
    uint8_t buf[] = {0x55, 0x0D, 0x02, 0x01, 0x00, 0x00, 0x00, 0xBB};
    Command.Judgment(buf);  //计算校验和
    ExtSRAMInterface.ExMem_Write_Bytes(0x6008, buf, 8);

    for (int i = 0; i < MAXIMUM_RETRY; ++i) {
        delay(WAITING_INTERVAL_MS);
        if (ExtSRAMInterface.ExMem_Read(0x6100) != 0x00) {
            // 解析层数信息
            ExtSRAMInterface.ExMem_Read_Bytes(buf, 8);
            uint8_t checksum = buf[6];    //获取校验和
            Command.Judgment(buf);
            if ((checksum == buf[6]) && (buf[0] == 0x55) && (buf[7] == 0xBB)) {
                return buf[4];
            }
        }
    }
    return 0;
}


void Road_Gate_TestA(const char* data) {
    uint8_t road_bufa[] = {0x55, 0x03, 0x10, 0x01, 0x00, 0x00, 0x00, 0xBB};
    uint8_t road_bufb[] = {0x55, 0x03, 0x11, 0x01, 0x00, 0x00, 0x00, 0xBB};
    road_bufa[3] = data[0];
    road_bufa[4] = data[1];
    road_bufa[5] = data[2];
    road_bufb[3] = data[3];
    road_bufb[4] = data[4];
    road_bufb[5] = data[5];
    Command.Judgment(road_bufa);  //计算校验和
    Command.Judgment(road_bufb);  //计算校验和

    delay(1000);
    ExtSRAMInterface.ExMem_Write_Bytes(0x6008, road_bufa, 8);
    delay(1000);
    ExtSRAMInterface.ExMem_Write_Bytes(0x6008, road_bufb, 8);
}




