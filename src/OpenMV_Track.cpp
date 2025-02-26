#include <Arduino.h>
#include "Command.h"
#include "ExtSRAMInterface.h"
#include "DCMotor.h"

uint8_t Data_OpenMVBuf[8];
char angle = 0;


uint8_t trackdi_buf[8] = {0x55, 0x02, 0x91, 0x00, 0x00, 0x00, 0x00, 0xBB};

/*
功    能：OpenMV巡线启动函数
参    数：无
返 回 值：无
*/
void OpenMVTrack_Disc_StartUp(void) {
    //   Servo_Control(-75);
    trackdi_buf[3] = 0x01;          //开始识别
    Command.Judgment(trackdi_buf);  //计算校验和
    ExtSRAMInterface.ExMem_Write_Bytes(0x6008, trackdi_buf, 8);
}

/*
功    能：关闭OpenMV巡线函数
参    数：无
返 回 值：无
*/
void OpenMVTrack_Disc_CloseUp(void) {
    //   Servo_Control(0);
    trackdi_buf[3] = 0x02;          //关闭识别
    Command.Judgment(trackdi_buf);  //计算校验和
    ExtSRAMInterface.ExMem_Write_Bytes(0x6008, trackdi_buf, 8);
    delay(500);
    Serial.print("SS");
}

/*
功    能：开启OpenMV转弯巡线启动函数
参    数：无
返 回 值：无
*/
void OpenMVTrack_Disc_StartUpZW(void) {
    //   Servo_Control(-75);
    trackdi_buf[3] = 0x04;          //开始识别
    Command.Judgment(trackdi_buf);  //计算校验和
    ExtSRAMInterface.ExMem_Write_Bytes(0x6008, trackdi_buf, 8);
}

/*
功    能：关闭OpenMV转弯巡线函数
参    数：无
返 回 值：无
*/
void OpenMVTrack_Disc_CloseUpZW(void) {
    //   Servo_Control(0);
    trackdi_buf[3] = 0x05;          //关闭识别
    Command.Judgment(trackdi_buf);  //计算校验和
    ExtSRAMInterface.ExMem_Write_Bytes(0x6008, trackdi_buf, 8);
}

void OpenMV_Stop(void) {
    DCMotor.Stop();
}

void OpenMV_Go(uint8_t speed, uint16_t dis) {
    DCMotor.Go(speed, dis);
}


void OpenMV_Track(uint8_t Car_Speed) {
    delay(500);
    Servo_Control(-45);
    delay(500);
    uint32_t num = 0;
    Car_Speed = 20;  // 基础速度
    float Kp = 0.6;  // 比例系数，根据实际情况调整
    float error, left_speed, right_speed;
    // 清空串口缓存
    while (ExtSRAMInterface.ExMem_Read(0x6038) != 0x00) {
        ExtSRAMInterface.ExMem_Read_Bytes(0x6038, Data_OpenMVBuf, 1);
    }
    delay(500);
    //发开始
    OpenMVTrack_Disc_StartUp();


    delay(500);
    ExtSRAMInterface.ExMem_Read_Bytes(0x6038, Data_OpenMVBuf, 9);
    ExtSRAMInterface.ExMem_Read_Bytes(0x6038, Data_OpenMVBuf, 9);
    DCMotor.SpeedCtr(Car_Speed, Car_Speed);

    while (1) {
        if (ExtSRAMInterface.ExMem_Read(0x6038) != 0x00)  // 检测OpenMV识别结果
        {
            ExtSRAMInterface.ExMem_Read_Bytes(0x6038, Data_OpenMVBuf, 9);
            if ((Data_OpenMVBuf[0] == 0x55) && (Data_OpenMVBuf[1] == 0x02) && (Data_OpenMVBuf[2] == 0x91)) {
                num++;
                Serial.println(num);
                if (Data_OpenMVBuf[4] == 1)  // 路口
                {
                    OpenMV_Stop();
                    OpenMVTrack_Disc_CloseUp();
                    delay(500);
                    DCMotor.Go(20, 300);
                    break;
                } else  // 调整
                {
                    // 根据偏航角计算误差
                    error = Data_OpenMVBuf[6] * ((Data_OpenMVBuf[5] == 43) ? 1 : -1);
                    // 应用比例控制
                    left_speed = Car_Speed - Kp * error;
                    right_speed = Car_Speed + Kp * error;
                    // 限制速度范围，防止超出可控范围
                    left_speed = constrain(left_speed, 0, 40);
                    right_speed = constrain(right_speed, 0, 40);
                    // 应用新的速度值
                    DCMotor.SpeedCtr(left_speed, right_speed);
                }
            }
        }
    }
}


void OpenMV_TrackCAR(uint8_t Car_Speed) {
    delay(500);
    Servo_Control(-45);
    delay(500);
    uint32_t num = 0;
    // Car_Speed = 40; // 基础速度
    float Kp = 0.4;  // 比例系数，根据实际情况调整
    float error, left_speed, right_speed;

    // 清空串口缓存
    while (ExtSRAMInterface.ExMem_Read(0x6038) != 0x00) {
        ExtSRAMInterface.ExMem_Read_Bytes(0x6038, Data_OpenMVBuf, 1);
    }
    delay(500);
    //发开始
    OpenMVTrack_Disc_StartUp();
    delay(500);
    ExtSRAMInterface.ExMem_Read_Bytes(0x6038, Data_OpenMVBuf, 9);
    ExtSRAMInterface.ExMem_Read_Bytes(0x6038, Data_OpenMVBuf, 9);
    DCMotor.SpeedCtr(Car_Speed, Car_Speed);

    while (1) {
        if (ExtSRAMInterface.ExMem_Read(0x6038) != 0x00)  // 检测OpenMV识别结果
        {
            ExtSRAMInterface.ExMem_Read_Bytes(0x6038, Data_OpenMVBuf, 9);
            if ((Data_OpenMVBuf[0] == 0x55) && (Data_OpenMVBuf[1] == 0x02) && (Data_OpenMVBuf[2] == 0x91)) {
                num++;
                Serial.println(num);
                if (Data_OpenMVBuf[4] == 1)  // 路口
                {
                    OpenMV_Stop();
                    OpenMVTrack_Disc_CloseUp();
                    delay(500);
                    DCMotor.Go(20, 400);
                    break;
                } else  // 调整
                {
                    // 根据偏航角计算误差
                    error = Data_OpenMVBuf[6] * ((Data_OpenMVBuf[5] == 43) ? 1 : -1);
                    // 应用比例控制
                    left_speed = Car_Speed - Kp * error;
                    right_speed = Car_Speed + Kp * error;
                    // 限制速度范围，防止超出可控范围
                    left_speed = constrain(left_speed, 0, 100);
                    right_speed = constrain(right_speed, 0, 100);
                    // 应用新的速度值
                    DCMotor.SpeedCtr(left_speed, right_speed);
                }
                if (Data_OpenMVBuf[7] == 1)  //有卡
                {
                    OpenMV_Stop();
                    delay(500);
                    DCMotor.Go(20, 100);
                    delay(500);
                    DCMotor.TurnLeft_Code(60, 150);
                    delay(1000);
                    ExtSRAMInterface.ExMem_Read_Bytes(0x6038, Data_OpenMVBuf, 9);
                    delay(100);
                    ExtSRAMInterface.ExMem_Read_Bytes(0x6038, Data_OpenMVBuf, 9);
                    delay(100);
                    ExtSRAMInterface.ExMem_Read_Bytes(0x6038, Data_OpenMVBuf, 9);
                    delay(100);
                    ExtSRAMInterface.ExMem_Read_Bytes(0x6038, Data_OpenMVBuf, 9);

                    if (Data_OpenMVBuf[4] == 1)  //判断当前是否在路口
                    {
                        delay(500);
                        OpenMVTrack_Disc_CloseUp();
                        delay(500);
                        DCMotor.Back(20, 100);
                        delay(500);
                        OpenMV_TrackRZWNow(50);
                        // DCMotor.TurnRight_Code(60, 150);
                        delay(500);
                        DCMotor.Go(20, 450);
                        break;
                    }
                    delay(500);
                    OpenMVTrack_Disc_CloseUp();
                    while (ExtSRAMInterface.ExMem_Read(0x6038) != 0x00) {
                        ExtSRAMInterface.ExMem_Read_Bytes(0x6038, Data_OpenMVBuf, 1);
                    }
                    delay(500);
                    // OpenMV_TrackRZWNow(50);
                    DCMotor.TurnRight_Code(60, 180);
                    delay(500);
                    Servo_Control(-45);
                    delay(1000);
                    DCMotor.Go(20, 350);
                    delay(1000);
                    OpenMVTrack_Disc_StartUp();
                    delay(500);
                    ExtSRAMInterface.ExMem_Read_Bytes(0x6038, Data_OpenMVBuf, 9);
                    DCMotor.SpeedCtr(Car_Speed, Car_Speed);
                }
            }
        }
    }
}

void OpenMV_back(uint8_t Car_Speed) {
    uint32_t num = 0;
    // Car_Speed = 40; // 基础速度
    float Kp = 0.7;  // 比例系数，根据实际情况调整
    float error, left_speed, right_speed;

    // 清空串口缓存
    while (ExtSRAMInterface.ExMem_Read(0x6038) != 0x00)
        ExtSRAMInterface.ExMem_Read_Bytes(0x6038, Data_OpenMVBuf, 1);


    //发开始
    OpenMVTrack_Disc_StartUp();


    delay(500);
    DCMotor.SpeedCtr(-Car_Speed, -Car_Speed);

    while (1) {
        if (ExtSRAMInterface.ExMem_Read(0x6038) != 0x00)  // 检测OpenMV识别结果
        {
            ExtSRAMInterface.ExMem_Read_Bytes(0x6038, Data_OpenMVBuf, 8);
            if ((Data_OpenMVBuf[0] == 0x55) && (Data_OpenMVBuf[1] == 0x02) && (Data_OpenMVBuf[2] == 0x91)) {
                num++;
                if (Data_OpenMVBuf[4] == 1)  // 路口
                {
                    OpenMV_Stop();
                    OpenMVTrack_Disc_CloseUp();
                    break;
                } else  // 调整
                {
                    // 根据偏航角计算误差
                    error = Data_OpenMVBuf[6] * ((Data_OpenMVBuf[5] == 43) ? 1 : -1);

                    // 应用比例控制
                    left_speed = -Car_Speed + Kp * error;
                    right_speed = -Car_Speed - Kp * error;

                    // 限制速度范围，防止超出可控范围
                    left_speed = constrain(left_speed, -100, 0);
                    right_speed = constrain(right_speed, -100, 0);

                    // 应用新的速度值
                    DCMotor.SpeedCtr(left_speed, right_speed);
                }
            }
        }
    }
    for (int i = 0; i < 8; i++) {
        Data_OpenMVBuf[i] = 0;
    }
}


void OpenMV_TrackLZW(uint8_t Car_Speed) {
    delay(500);
    Servo_Control(-75);
    delay(500);
    DCMotor.SpeedCtr(-Car_Speed, Car_Speed);
    delay(500);
    OpenMVTrack_Disc_StartUpZW();
    ExtSRAMInterface.ExMem_Read_Bytes(0x6038, Data_OpenMVBuf, 8);
    ExtSRAMInterface.ExMem_Read_Bytes(0x6038, Data_OpenMVBuf, 8);
    while (1) {
        if (ExtSRAMInterface.ExMem_Read(0x6038) != 0x00)  // 检测OpenMV识别结果
        {
            ExtSRAMInterface.ExMem_Read_Bytes(0x6038, Data_OpenMVBuf, 8);
            if ((Data_OpenMVBuf[0] == 0x55) && (Data_OpenMVBuf[1] == 0x02) && (Data_OpenMVBuf[2] == 0x91)) {
                if (Data_OpenMVBuf[4] == 1)  // 转弯到了中心
                {
                    OpenMV_Stop();
                    OpenMVTrack_Disc_CloseUpZW();
                    DCMotor.Back(20, 200);
                    break;
                }
            }
        }
    }
}

void OpenMV_TrackRZW(uint8_t Car_Speed) {
    delay(500);
    Servo_Control(-75);
    delay(500);
    DCMotor.SpeedCtr(Car_Speed, -Car_Speed);
    delay(500);
    OpenMVTrack_Disc_StartUpZW();
    ExtSRAMInterface.ExMem_Read_Bytes(0x6038, Data_OpenMVBuf, 8);
    ExtSRAMInterface.ExMem_Read_Bytes(0x6038, Data_OpenMVBuf, 8);
    while (1) {
        if (ExtSRAMInterface.ExMem_Read(0x6038) != 0x00)  // 检测OpenMV识别结果
        {
            ExtSRAMInterface.ExMem_Read_Bytes(0x6038, Data_OpenMVBuf, 8);
            if ((Data_OpenMVBuf[0] == 0x55) && (Data_OpenMVBuf[1] == 0x02) && (Data_OpenMVBuf[2] == 0x91)) {
                if (Data_OpenMVBuf[4] == 1)  // 转弯到了中心
                {
                    OpenMV_Stop();
                    OpenMVTrack_Disc_CloseUpZW();
                    DCMotor.Back(20, 200);
                    break;
                }
            }
        }
    }
}


void OpenMV_TrackRZWNow(uint8_t Car_Speed) {
    delay(500);
    Servo_Control(-75);
    delay(500);
    DCMotor.SpeedCtr(Car_Speed, -Car_Speed);
    delay(100);
    OpenMVTrack_Disc_StartUpZW();
    ExtSRAMInterface.ExMem_Read_Bytes(0x6038, Data_OpenMVBuf, 8);
    ExtSRAMInterface.ExMem_Read_Bytes(0x6038, Data_OpenMVBuf, 8);
    while (1) {
        if (ExtSRAMInterface.ExMem_Read(0x6038) != 0x00)  // 检测OpenMV识别结果
        {
            ExtSRAMInterface.ExMem_Read_Bytes(0x6038, Data_OpenMVBuf, 8);
            if ((Data_OpenMVBuf[0] == 0x55) && (Data_OpenMVBuf[1] == 0x02) && (Data_OpenMVBuf[2] == 0x91)) {
                if (Data_OpenMVBuf[4] == 1)  // 转弯到了中心
                {
                    OpenMV_Stop();
                    OpenMVTrack_Disc_CloseUpZW();
                    break;
                }
            }
        }
    }
}





//Android通过主车设置舵机角度

void Servoangle_control(uint8_t data) {
    if (data == 0x2D)  //判断副指令第二位为上升还是下降
    {
        angle = angle - 5;             //在原始数据减5
        if (angle < -80) angle = -80;  //减到-80，直接控制在-80

    } else if (data == 0x2B) {
        angle = angle + 5;
        if (angle > 20) angle = 20;
    } else if (data == 0x2C) {
        angle = 0;
    }
    delay(100);
    Servo_Control(angle);
}
