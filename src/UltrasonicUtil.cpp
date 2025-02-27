#include "UltrasonicUtil.h"
#include "Ultrasonic.h"


double ultrasonic_ranging(int sys, int rangeFrequency, int wait) {
    double distance = 0;
    for (int i = 0; i < rangeFrequency; ++i) {
        distance += Ultrasonic.Ranging(CM);
        delay(wait);
    }
    return distance / rangeFrequency;
}


void ultrasonic_adjustDistance(uint8_t carSeep, uint8_t targetDistance, double errorMargin, unsigned long controlTime_ms, int wait) {
    ulong startTime = millis();
    while (millis() - startTime >= controlTime_ms) {
        DCMotor.Stop();
        double distance = ultrasonic_ranging(CM);
        double distanceError = distance - targetDistance;
        if (abs(distanceError) > errorMargin) {
            if (distanceError > errorMargin) {
                DCMotor.Go(carSeep);
            } else {
                DCMotor.Back(carSeep);
            }
        }
        delay(wait);
    }
    DCMotor.Stop();
}


uint8_t LED_display_buf[] = {0x55, 0x04, 0x04, 0x00, 0x00, 0x00, 0x00, 0xbb};

/*
超声波测距
*/
/*
void chaoShengBo(void) {
    distance_new = 0;                             // 重置距离为零
    for (csbcjcs = 0; csbcjcs < 20; csbcjcs++) {  // 进行20次测距，并累加结果
        distance_new += Ultrasonic.Ranging(1);      // 进行一次超声波测距，并将结果累加到distance_new中
        delay(50);                                  // 等待一段时间
    }
    distance_new = distance_new / 20.0 * 10;   // 计算距离的平均值
    Serial.println(distance_new);              // 将平均距离打印到串口
    int intValue = (int) (distance_new * 100);  // 将测量结果乘以100，并转换为整数
    Serial.println(intValue);                  // 打印整数形式的测量结果到串口

    // 获取整数的百位和十位数字
    int hundredthDigit = intValue / 10000;      // 获取百位数字
    int geredthDigit = intValue % 10000 / 100;  // 获取十位数字
    Serial.println(geredthDigit);               // 打印十位数字到串口

    // 将百位和十位数字存储到 carLight 显示缓冲区中
    LED_display_buf[4] = hundredthDigit;  // 存储百位数字
    LED_display_buf[5] = geredthDigit;    // 存储十位数字

    Command.Judgment(LED_display_buf);                               // 调用命令处理函数，处理 carLight 显示缓冲区
    ExtSRAMInterface.ExMem_Write_Bytes(0x6008, LED_display_buf, 8);  // 将处理结果写入外部存储器
}
*/
