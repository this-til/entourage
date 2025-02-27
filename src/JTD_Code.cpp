/*
#include <Arduino.h>
#include "Command.h"
#include "ExtSRAMInterface.h"

*/
/*
交通灯A进入识别模式
参数：无
返回值：无
*//*

uint8_t trafficA_enter_command[] = {0x55, 0x0E, 0x01, 0x00, 0x00, 0x00, 0x00, 0xBB};

void GateA_enter_Test(void) {
    Command.Judgment(trafficA_enter_command);  // 计算校验和
    ExtSRAMInterface.ExMem_Write_Bytes(0x6008, trafficA_enter_command, 8);
}

*/
/*
交通灯A颜色确认测试
参数：颜色 01红色 02绿色 03黄色
返回值：无
*//*

uint8_t trafficA_color_command[] = {0x55, 0x0E, 0x02, 0x00, 0x00, 0x00, 0x00, 0xBB};

void Road_GateA_Test(uint8_t color) {
    trafficA_color_command[3] = color;
    Command.Judgment(trafficA_color_command);  // 计算校验和
    ExtSRAMInterface.ExMem_Write_Bytes(0x6008, trafficA_color_command, 8);
}

*/
/*
交通灯B进入识别模式
参数：无
返回值：无
*//*

uint8_t trafficB_enter_command[] = {0x55, 0x0F, 0x01, 0x00, 0x00, 0x00, 0x00, 0xBB};

void GateB_enter_Test(void) {
    Command.Judgment(trafficB_enter_command);  // 计算校验和
    ExtSRAMInterface.ExMem_Write_Bytes(0x6008, trafficB_enter_command, 8);
}

*/
/*
交通灯B颜色确认测试
参数：颜色 01红色 02绿色 03黄色
返回值：无
*//*

uint8_t trafficB_color_command[] = {0x55, 0x0F, 0x02, 0x00, 0x00, 0x00, 0x00, 0xBB};

void Road_GateB_Test(uint8_t color) {
    trafficB_color_command[3] = color;
    Command.Judgment(trafficB_color_command);  // 计算校验和
    ExtSRAMInterface.ExMem_Write_Bytes(0x6008, trafficB_color_command, 8);
}

*/
/*
交通灯C进入识别模式
参数：无
返回值：无
*//*

uint8_t trafficC_enter_command[] = {0x55, 0x13, 0x01, 0x00, 0x00, 0x00, 0x00, 0xBB};

void GateC_enter_Test(void) {
    Command.Judgment(trafficC_enter_command);  // 计算校验和
    ExtSRAMInterface.ExMem_Write_Bytes(0x6008, trafficC_enter_command, 8);
}

*/
/*
交通灯C颜色确认测试
参数：颜色 01红色 02绿色 03黄色
返回值：无
*//*

uint8_t trafficC_color_command[] = {0x55, 0x13, 0x02, 0x00, 0x00, 0x00, 0x00, 0xBB};

void Road_GateC_Test(uint8_t color) {
    trafficC_color_command[3] = color;
    Command.Judgment(trafficC_color_command);  // 计算校验和
    ExtSRAMInterface.ExMem_Write_Bytes(0x6008, trafficC_color_command, 8);
}


*/
/*
交通灯识别 交通灯A
*//*

void Road_Traffic_ATest(void) {
    delay(500);
    Servo_Control(20);
    K210_Send(0x05);
    delay(6000);
    int GateB = 0;
    GateA_enter_Test();  //交通灯B进入识别模式
    delay(1000);
    GateA_enter_Test();  //交通灯B进入识别模式
    delay(100);
    GateA_enter_Test();  //交通灯B进入识别模式
    delay(2000);
    K210_Send(0x03);  //开启交通灯识别
    delay(500);
    while (1) {
        if (ExtSRAMInterface.ExMem_Read(0x6038) != 0x00)  //检测OpenMV识别结果
        {
            delay(500);
            Data_Type = ExtSRAMInterface.ExMem_Read(0x603A);
            Data_Flag = ExtSRAMInterface.ExMem_Read(0x603B);
            Data_Length = ExtSRAMInterface.ExMem_Read(0x603C);
            Data_Length = Data_Length + 6;
            ExtSRAMInterface.ExMem_Read_Bytes(0x6038, Data_OTABuf, Data_Length);
            ExtSRAMInterface.ExMem_Read_Bytes(0x6038, Data_OTABuf, Data_Length);
            ExtSRAMInterface.ExMem_Read_Bytes(0x6038, Data_OTABuf, Data_Length);
            ExtSRAMInterface.ExMem_Read_Bytes(0x6038, Data_OTABuf, Data_Length);
            if ((Data_OTABuf[0] == 0x55) && (Data_OTABuf[1] == 0x02)) {

                if (Data_OTABuf[2] = 0x92) {
                    Serial.println(Data_OTABuf[3]);
                    if (Data_OTABuf[3] == 0x05) {
                        Serial.println("红色");
                        K210_Send(0x04);        //关闭交通灯识别
                        Road_GateA_Test(0x01);  //交通事A红色确认
                    } else if (Data_OTABuf[3] == 0x06) {
                        Serial.println("绿色");
                        K210_Send(0x04);        //关闭交通灯识别
                        Road_GateA_Test(0x02);  //交通事A绿色确认
                    } else if (Data_OTABuf[3] == 0x07) {
                        Serial.println("黄色");
                        K210_Send(0x04);        //关闭交通灯识别
                        Road_GateA_Test(0x03);  //交通事A黄色确认
                    }
                    delay(200);
                    for (int i = 0; i < 8; i++) {
                        Data_OTABuf[i] = 0;
                    }
                    break;
                }
            }
        }
        delay(1);
        GateB++;
        if (GateB > 10000) {
            break;
        }
    }
    K210_Send(0x07);
    delay(6000);
}


*/
/*
交通灯识别 交通灯B
*//*

void Road_Traffic_BTest(void) {
    delay(500);
    Servo_Control(20);
    delay(500);
    K210_Send(0x05);
    delay(8000);
    int GateB = 0;
    GateB_enter_Test();  // 交通灯B进入识别模式
    delay(1000);
    GateB_enter_Test();  // 交通灯B进入识别模式
    delay(100);
    GateB_enter_Test();  // 交通灯B进入识别模式
    delay(2000);
    K210_Send(0x03);  // 开启交通灯识别
    delay(500);
    while (1) {
        if (ExtSRAMInterface.ExMem_Read(0x6038) != 0x00)  // 检测OpenMV识别结果
        {
            delay(500);
            Data_Type = ExtSRAMInterface.ExMem_Read(0x603A);
            Data_Flag = ExtSRAMInterface.ExMem_Read(0x603B);
            Data_Length = ExtSRAMInterface.ExMem_Read(0x603C);
            Data_Length = Data_Length + 6;
            ExtSRAMInterface.ExMem_Read_Bytes(0x6038, Data_OTABuf, Data_Length);
            ExtSRAMInterface.ExMem_Read_Bytes(0x6038, Data_OTABuf, Data_Length);
            ExtSRAMInterface.ExMem_Read_Bytes(0x6038, Data_OTABuf, Data_Length);
            ExtSRAMInterface.ExMem_Read_Bytes(0x6038, Data_OTABuf, Data_Length);
            if ((Data_OTABuf[0] == 0x55) && (Data_OTABuf[1] == 0x02)) {

                if (Data_OTABuf[2] = 0x92) {
                    Serial.println(Data_OTABuf[3]);
                    if (Data_OTABuf[3] == 0x05) {
                        Serial.println("红色");
                        K210_Send(0x04);        // 关闭交通灯识别
                        Road_GateB_Test(0x01);  // 交通灯B红色确认
                    } else if (Data_OTABuf[3] == 0x06) {
                        Serial.println("绿色");
                        K210_Send(0x04);        // 关闭交通灯识别
                        Road_GateB_Test(0x02);  // 交通灯B绿色确认
                    } else if (Data_OTABuf[3] == 0x07) {
                        Serial.println("黄色");
                        K210_Send(0x04);        // 关闭交通灯识别
                        Road_GateB_Test(0x03);  // 交通灯B黄色确认
                    }
                    delay(200);
                    for (int i = 0; i < 8; i++) {
                        Data_OTABuf[i] = 0;
                    }
                    break;
                }
            }
        }
        delay(1);
        GateB++;
        if (GateB > 10000) {
            break;
        }
    }
    K210_Send(0x07);
    delay(8000);
}
///////

// 1:355mm
// ABC356
// F7
// 3d:ZHUOZHUAN
// 1dang


// 2:320mm
// BEF589
// RUKU:F7
// zhuozhuan
// 4dang




*/
/*
交通灯识别 交通灯C
*//*

void Road_Traffic_CTest(void) {
    int GateC = 0;
    GateC_enter_Test();  // 交通灯C进入识别模式
    delay(1000);
    GateC_enter_Test();  // 交通灯C进入识别模式
    delay(100);
    GateC_enter_Test();  // 交通灯C进入识别模式
    delay(2000);
    K210_Send(0x03);  // 开启交通灯识别
    delay(500);
    while (1) {
        if (ExtSRAMInterface.ExMem_Read(0x6038) != 0x00)  // 检测OpenMV识别结果
        {
            delay(500);
            Data_Type = ExtSRAMInterface.ExMem_Read(0x603A);
            Data_Flag = ExtSRAMInterface.ExMem_Read(0x603B);
            Data_Length = ExtSRAMInterface.ExMem_Read(0x603C);
            Data_Length = Data_Length + 6;
            ExtSRAMInterface.ExMem_Read_Bytes(0x6038, Data_OTABuf, Data_Length);
            ExtSRAMInterface.ExMem_Read_Bytes(0x6038, Data_OTABuf, Data_Length);
            ExtSRAMInterface.ExMem_Read_Bytes(0x6038, Data_OTABuf, Data_Length);
            ExtSRAMInterface.ExMem_Read_Bytes(0x6038, Data_OTABuf, Data_Length);
            if ((Data_OTABuf[0] == 0x55) && (Data_OTABuf[1] == 0x02)) {

                if (Data_OTABuf[2] = 0x92) {
                    Serial.println(Data_OTABuf[3]);
                    if (Data_OTABuf[3] == 0x05) {
                        Serial.println("红色");
                        K210_Send(0x04);        // 关闭交通灯识别
                        Road_GateC_Test(0x01);  // 交通灯C红色确认
                    } else if (Data_OTABuf[3] == 0x06) {
                        Serial.println("绿色");
                        K210_Send(0x04);        // 关闭交通灯识别
                        Road_GateC_Test(0x02);  // 交通灯C绿色确认
                    } else if (Data_OTABuf[3] == 0x07) {
                        Serial.println("黄色");
                        K210_Send(0x04);        // 关闭交通灯识别
                        Road_GateC_Test(0x03);  // 交通灯C黄色确认
                    }
                    delay(200);
                    for (int i = 0; i < 8; i++) {
                        Data_OTABuf[i] = 0;
                    }
                    break;
                }
            }
        }
        delay(1);
        GateC++;
        if (GateC > 10000) {
            break;
        }
    }
}


*/
