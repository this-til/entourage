#include "Garage.h"

//=================以下为车库封装============================

enum Garage {
    A = 1,
    B = 2
};


/*
车库A一层测试
*/
uint8_t carparkA1_buf[] = {0x55, 0x0D, 0x01, 0x01, 0x01, 0x00, 0x00, 0xBB};

void carparkA1_Test(void) {
    Command.Judgment(carparkA1_buf);  //计算校验和
    ExtSRAMInterface.ExMem_Write_Bytes(0x6008, carparkA1_buf, 8);
}

/*
车库A二层测试
*/
uint8_t carparkA2_buf[] = {0x55, 0x0D, 0x01, 0x02, 0x00, 0x00, 0x00, 0xBB};

void carparkA2_Test(void) {
    Command.Judgment(carparkA2_buf);  //计算校验和
    ExtSRAMInterface.ExMem_Write_Bytes(0x6008, carparkA2_buf, 8);
}

/*
车库A三层测试
*/
uint8_t carparkA3_buf[] = {0x55, 0x0D, 0x01, 0x03, 0x00, 0x00, 0x00, 0xBB};

void carparkA3_Test(void) {
    Command.Judgment(carparkA3_buf);  //计算校验和
    ExtSRAMInterface.ExMem_Write_Bytes(0x6008, carparkA3_buf, 8);
}

/*
车库A四层测试
*/
uint8_t carparkA4_buf[] = {0x55, 0x0D, 0x01, 0x04, 0x00, 0x00, 0x00, 0xBB};

void carparkA4_Test(void) {
    Command.Judgment(carparkA4_buf);  //计算校验和
    ExtSRAMInterface.ExMem_Write_Bytes(0x6008, carparkA4_buf, 8);
}

//================以上为车库封装============================

/*
请求返回立体车库当前层数
*/

uint8_t carrequest_buf[] = {0x55, 0x0D, 0x02, 0x01, 0x00, 0x00, 0x00, 0xBB};

void carrequest_Test(void) {
    Command.Judgment(carrequest_buf);  //计算校验和
    ExtSRAMInterface.ExMem_Write_Bytes(0x6008, carrequest_buf, 8);
}

/*
识别车库层数
*/
uint8_t Rbuf[8];
uint8_t floorNumber;  // 修改变量名为 floorNumber

void ckcscx() {
    cka = receiveGarageCommand();
}

int receiveGarageCommand() {
    // 检查指令是否为回传指令
    carrequest_Test();
    delay(200);
    if (ExtSRAMInterface.ExMem_Read(0x6100) != 0x00) {

        // 解析层数信息
        ExtSRAMInterface.ExMem_Read_Bytes(Rbuf, 8);
        Rbuf_judge = Rbuf[6];    //获取校验和
        Command.Judgment(Rbuf);  //计算校验和
        if ((Rbuf_judge == Rbuf[6]) && (Rbuf[0] == 0x55) && (Rbuf[7] == 0xBB)) {
            floorNumber = Rbuf[4];  // 修改赋值的变量名为 floorNumber
            // 根据层数执行相应操作
            switch (floorNumber) {  // 修改 switch 语句中的变量名为 floorNumber
                case 0x01:
                    return 1;
                    break;
                case 0x02:
                    return 2;
                    break;
                case 0x03:
                    return 3;
                    break;
                case 0x04:
                    return 4;
                    break;
                default:
                    // 未知层数处理逻辑
                    // TODO: 添加您的代码
                    break;
            }
        }
    }
}

void A1_Cat_ku_wait() {
    while (1) {
        int a;
        a = receiveGarageCommand();
        if (a == 1) {
            break;
        }
        delay(500);
        carparkA1_Test();
        delay(500);
    }
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




