#include "UltrasonicCode.h"


#define RANGE_FREQUENCY 20

double disa;
uint8_t LED_display_buf[] = {0x55, 0x04, 0x04, 0x00, 0x00, 0x00, 0x00, 0xbb};


double ultrasonicRanging();

/***
 * 超声波测距
 * @return
 */
double ultrasonicRanging() {
    double distance = 0;
    for (int i = 0; i < RANGE_FREQUENCY; ++i) {
        distance += Ultrasonic.Ranging(CM);
        delay(50);
    }
    return distance / RANGE_FREQUENCY;
}


void csbkz(uint8_t Car_Speed, uint8_t Car_disa) {
    while (1) {
        //读取超声波距离
        disa = Ultrasonic.Ranging(CM);
        Serial.print(disa, 1);
        Serial.println(" cm");
        //如果超声波距离大于设定距离，就后退到超声波距离停止
        if (disa > Car_disa) {
            DCMotor.Back(45);
            DCMotor.Stop();
        }
        if (disa < Car_disa) {
            DCMotor.Go(45);
            DCMotor.Stop();
        }
    }
}

void maintainDistance(uint8_t carSpeed, uint8_t targetDistance) {
    const float errorMargin = 2.0;        // 允许的误差范围（单位：厘米）
    unsigned long startTime = 0;          // 开始计时的时间
    const unsigned long duration = 2000;  // 持续时间（单位：毫秒，这里设置为3秒）

    while (true) {
        float currentDistance = Ultrasonic.Ranging(CM);
        Serial.print(currentDistance, 1);
        Serial.println(" cm");

        float distanceError = currentDistance - targetDistance;

        // 检查误差是否在允许范围内
        if (abs(distanceError) <= errorMargin) {
            DCMotor.Stop();  // 停止电机
            // 如果误差刚进入允许范围，开始计时
            if (startTime == 0) {
                startTime = millis();
            }
                // 如果误差持续在允许范围内超过3秒，退出循环
            else if (millis() - startTime >= duration) {
                DCMotor.Stop();  // 停止电机
                break;
            }
        } else {
            // 如果误差超出允许范围，重置计时器并调整电机速度
            startTime = 0;
            if (distanceError > errorMargin) {
                DCMotor.Go(carSpeed);
            } else {
                DCMotor.Back(carSpeed);
            }
        }

        delay(50);  // 减少测量频率
    }
}

/*
超声波测距
*/
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

    // 将百位和十位数字存储到 LED 显示缓冲区中
    LED_display_buf[4] = hundredthDigit;  // 存储百位数字
    LED_display_buf[5] = geredthDigit;    // 存储十位数字

    Command.Judgment(LED_display_buf);                               // 调用命令处理函数，处理 LED 显示缓冲区
    ExtSRAMInterface.ExMem_Write_Bytes(0x6008, LED_display_buf, 8);  // 将处理结果写入外部存储器
}
