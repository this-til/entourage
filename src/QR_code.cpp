

/////////////////////////////////////////////////////////////////////////////////

#include <HardwareSerial.h>
#include <Arduino.h>
#include "DCMotor.h"
#include "BEEP.h"
#include "ExtSRAMInterface.h"

/*
二维码颜色功能识别
*/
char DZ_data[6];
uint8_t Data_ys[40];
uint8_t cs[40];
int xx;

/*
静态标志物（B）中有两个二维码信息，均需识别，选手可
根据二维码信息中固定字节长度进行区分。
二维码（1）中信息为固定 6 个字节长度的字符串,仅包含
数字（0~9）。
二维码（2）中信息为固定 10 个字节长度的字符串，仅包
含数字（0~9）和大写字母（A~Z）。
二维码最终有效数据为根据二维码（1）中得到的数字信息
依次对二维码（2）数据中指定位进行提取得到的数据。
示例：二维码（1）信息为“014684”，二维码（2）中信息
为“AE1D6Y8FDS”，将二维码（2）中第 0 位、第 1 位、第 4 位、
第 6 位、第 8 位和第 4 位提取出来后得到“AE68D6”。最终通
过将“AE68D6”转化为 ASCII 码得到“0x41，0x45，0x36，0x38，
0x44，0x36”作为烽火台报警标志物开启码。
*/

void QR72022(String str1, String str2) {
    String Plate = "";

    // 获取str1对应位置的字符，并从str2中获取相应的字符构成Plate
    for (int i = 0; i < str1.length(); ++i) {
        char ch = str1.charAt(i);
        int shunxu = ch - '0';  // 将字符转换为数字
        char ch2 = str2.charAt(shunxu);
        Plate += ch2;
    }

    // 输出转换后的字符串
    Serial.println(Plate);

    // 将String类型的Plate转换为char数组
    char cha[Plate.length() + 1];  // 要为字符串结尾的空字符留一个位置
    Plate.toCharArray(cha, Plate.length() + 1);

    // 输出转换后的字符数组的 ASCII 码值
    Serial.print("ASCII codes: ");
    for (int i = 0; i < strlen(cha); ++i) {
        Serial.print(cha[i], HEX);
    }
    Serial.println();
}


int sumDigits(const char* str) {
    int sum = 0;
    while (*str) {                       // 遍历字符串直到遇到字符串结束符 '\0'
        if (*str >= '0' && *str <= '9') {  // 如果当前字符是数字字符
            sum += *str - '0';               // 将字符转换为相应的数字并累加到总和中
        }
        str++;  // 移动到下一个字符
    }
    return sum;
}

String moveUpperCaseChars(const char* str) {
    String result = "";

    for (int i = 0; str[i] != '\0'; i++) {
        if (isupper(str[i])) {
            result += (char) ((str[i] - 'A' + 3) % 26 + 'A');  // 移动3个位置
        } else {
            result += str[i];
        }
    }

    return result;
}


void extractCarPlate(String rawData) {
    char cp[rawData.length() + 1];  // 考虑到字符串末尾的空字符 '\0'
    for (int i = 0; i < rawData.length(); ++i) {
        cp[i] = rawData.charAt(i);
    }
    cp[rawData.length()] = '\0';  // 手动添加字符串末尾的空字符 '\0'
}


// 处理车牌信息二维码
String extractCarPlateInfo(String rawData) {
    BEEP.TurnOn();
    delay(50);
    BEEP.TurnOff();
    delay(50);
    BEEP.TurnOn();
    delay(50);
    BEEP.TurnOff();
    String carPlate = "";
    for (int i = 0; i < rawData.length(); ++i) {
        char ch = rawData.charAt(i);
        // 检查字符是否为大写字母或数字
        if ((ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9')) {
            carPlate += ch;
        }
    }
    return carPlate;
}

// 处理计算公式二维码
// 对于包含数学运算符的二维码，我们可以简单地返回原始字符串，因为不需要去除干扰字符：
String extractFormula(String rawData) {
    return rawData;
}


// 这个函数用于启动二维码识别并处理结果
void Road_QR_Test(void) {
    int numberOfQRCodesExpected = 0;  // 预期接收的二维码数量
    int numberOfQRCodesReceived = 0;  // 已接收的二维码数量
    int i = 0, red = 0, bulu = 0, green = 0;;  // 超时计数器
    delay(100);
    Servo_Control(10);
    delay(500);
    DCMotor.TurnLeft_Code(40, 90);
    // 开启二维码识别

    for (int i = 0; i < 10; i++) {
        K210_Send(0x01);
        delay(1000);
        while (true) {
            // 检查是否有数据可读
            if (ExtSRAMInterface.ExMem_Read(0x6038) != 0x00) {
                // 读取数据类型、标志和长度
                uint8_t Data_Type = ExtSRAMInterface.ExMem_Read(0x603A);
                uint8_t Data_Flag = ExtSRAMInterface.ExMem_Read(0x603B);
                uint8_t Data_Length = ExtSRAMInterface.ExMem_Read(0x603C);
                uint8_t dataLength = ExtSRAMInterface.ExMem_Read(0x603C) + 6;
                delay(100);
                ExtSRAMInterface.ExMem_Read_Bytes(0x6038, Data_OTABuf, dataLength);

                if ((Data_OTABuf[0] == 0x55) && (Data_OTABuf[1] == 0x02)) {
                    switch (Data_OTABuf[3]) {
                        case 0x00:  // 接收到二维码数量
                            numberOfQRCodesExpected = Data_OTABuf[4];
                            Serial.print("Number of QR Codes: ");
                            Serial.println(numberOfQRCodesExpected);
                            while (true) {
                                if (ExtSRAMInterface.ExMem_Read(0x6038) != 0x00) {
                                    // 读取数据类型、标志和长度
                                    uint8_t Data_Type = ExtSRAMInterface.ExMem_Read(0x603A);
                                    uint8_t Data_Flag = ExtSRAMInterface.ExMem_Read(0x603B);
                                    uint8_t Data_Length = ExtSRAMInterface.ExMem_Read(0x603C);
                                    uint8_t dataLength = ExtSRAMInterface.ExMem_Read(0x603C) + 6;
                                    ExtSRAMInterface.ExMem_Read_Bytes(0x6038, Data_OTABuf, dataLength);
                                    delay(100);
                                    if ((Data_OTABuf[0] == 0x55) && (Data_OTABuf[1] == 0x02) &&
                                        (Data_OTABuf[3] == 0x01)) {
                                        String qrCodeData = "";
                                        for (int i = 6; i < Data_Length + 6; ++i) {
                                            qrCodeData += (char) Data_OTABuf[i];
                                        }
                                        String cleanedData = "";
                                        // if (qrCodeData.length() == 16) {  // 假设车牌信息长度为8
                                        //   cleanedData = extractCarPlateInfo(qrCodeData);
                                        //   xx = sumDigits(qrCodeData.c_str());
                                        //   qrCodeDataArray[0] = cleanedData;
                                        // } else if (qrCodeData.length() == 12) {
                                        //   cleanedData = extractFormula(qrCodeData.c_str());
                                        //   qrCodeDataArray[2] = cleanedData;
                                        // } else {
                                        //   cleanedData = extractFormula(qrCodeData);
                                        //   qrCodeDataArray[1] = cleanedData;
                                        // }


                                        if (qrCodeData.length() == 8) {  // 假设车牌信息长度为8
                                            cleanedData = extractCarPlateInfo(qrCodeData);
                                            // xx = sumDigits(qrCodeData.c_str());
                                            qrCodeDataArray[0] = cleanedData;
                                            Serial.print("车牌：");
                                            Serial.println(qrCodeDataArray[0]);
                                        } else {
                                            cleanedData = extractFormula(qrCodeData);
                                            qrCodeDataArray[1] = cleanedData;
                                            Serial.print("公式：");
                                            Serial.println(qrCodeDataArray[1]);
                                        }

                                        Serial.print("Processed QR Code Data: ");
                                        Serial.println(cleanedData);
                                        // ... 保存或处理cleanedData ...
                                        numberOfQRCodesReceived++;
                                    }
                                }
                                if (numberOfQRCodesExpected != 0 &&
                                    numberOfQRCodesReceived >= numberOfQRCodesExpected) {
                                    break;
                                }
                            }
                            break;
                    }
                    if (numberOfQRCodesExpected == numberOfQRCodesReceived) {
                        break;
                    }
                }
            }
            BEEP.TurnOn();
            delay(50);
            BEEP.TurnOff();
            DCMotor.TurnRight_Code(40, 40);
            K210_Send(0x02);
            delay(3000);
            numberOfQRCodesExpected = 0;
            numberOfQRCodesReceived = 0;  // 修改此行，重置已接收的二维码数量
            break;
        }
    }
    Serial.print("第一个二维码");
    Serial.println(qrCodeDataArray[0]);
    // setGlobalVariable0328(qrCodeDataArray[2]);
    Serial.print("第二个二维码");
    Serial.println(qrCodeDataArray[1]);
    Serial.print("第三个二维码");
    Serial.println(qrCodeDataArray[2]);
    // 关闭二维码识别
    K210_Send(0x02);
    DCMotor.TurnLeft_Code(40, 90);
}


//计算公式
//
int tihuan(const char* str, int n, int y, int r) {
    char expr[100];  // 存放替换后的表达式
    int len = strlen(str);
    int j = 0;  // 表达式数组的索引

    // 复制替换后的表达式到expr数组中
    for (int i = 0; i < len; ++i) {
        if (str[i] == 'n') {  // 替换n
            j += sprintf(expr + j, "%d", n);
        } else if (str[i] == 'y') {  // 替换y
            j += sprintf(expr + j, "%d", y);
        } else if (str[i] == 'r') {  // 替换r
            j += sprintf(expr + j, "%d", r);
        } else {
            expr[j++] = str[i];  // 将非变量字符直接复制到expr中
        }
    }
    expr[j] = '\0';  // 确保字符串结束

    Serial.println(expr);  // 打印替换后的表达式，用于调试

    // 这里你可以调用你的表达式计算函数来计算替换后的表达式的值
    int result = postfix(expr);

    Serial.println(result);

    // 返回计算结果
    return result;
}


int compareWithStringArray(String input, String array[], int arrayLength) {
    for (int i = 0; i < arrayLength; ++i) {
        if (input.equals(array[i])) {
            return 1;  // 如果找到相同的字符串，返回1
        }
    }
    return 2;  // 如果没有找到相同的字符串，返回2
}


void Road_QR_Test1(void) {
    int numberOfQRCodesExpected = 0;  // 预期接收的二维码数量
    int numberOfQRCodesReceived = 0;  // 已接收的二维码数量
    int i = 0, red = 0, bulu = 0, green = 0;
    int shulaing = 0;;  // 超时计数器

    DCMotor.TurnLeft_Code(40, 120);
    // 开启二维码识别
    String qrCodelsData1[10];

    for (int i = 0; i < 13; i++) {
        K210_Send(0x01);
        delay(1000);
        while (true) {
            // 检查是否有数据可读
            if (ExtSRAMInterface.ExMem_Read(0x6038) != 0x00) {
                // 读取数据类型、标志和长度
                uint8_t Data_Type = ExtSRAMInterface.ExMem_Read(0x603A);
                uint8_t Data_Flag = ExtSRAMInterface.ExMem_Read(0x603B);
                uint8_t Data_Length = ExtSRAMInterface.ExMem_Read(0x603C);
                uint8_t dataLength = ExtSRAMInterface.ExMem_Read(0x603C) + 6;
                delay(100);
                ExtSRAMInterface.ExMem_Read_Bytes(0x6038, Data_OTABuf, dataLength);

                if ((Data_OTABuf[0] == 0x55) && (Data_OTABuf[1] == 0x02)) {
                    switch (Data_OTABuf[3]) {
                        case 0x00:  // 接收到二维码数量
                            numberOfQRCodesExpected = Data_OTABuf[4];
                            Serial.print("Number of QR Codes: ");
                            Serial.println(numberOfQRCodesExpected);
                            while (true) {
                                if (ExtSRAMInterface.ExMem_Read(0x6038) != 0x00) {
                                    // 读取数据类型、标志和长度
                                    uint8_t Data_Type = ExtSRAMInterface.ExMem_Read(0x603A);
                                    uint8_t Data_Flag = ExtSRAMInterface.ExMem_Read(0x603B);
                                    uint8_t Data_Length = ExtSRAMInterface.ExMem_Read(0x603C);
                                    uint8_t dataLength = ExtSRAMInterface.ExMem_Read(0x603C) + 6;
                                    ExtSRAMInterface.ExMem_Read_Bytes(0x6038, Data_OTABuf, dataLength);
                                    delay(100);
                                    if ((Data_OTABuf[0] == 0x55) && (Data_OTABuf[1] == 0x02) &&
                                        (Data_OTABuf[3] == 0x01)) {
                                        switch (Data_OTABuf[5]) {
                                            case 0x55:
                                                Serial.print("紅色");
                                                red++;
                                                break;
                                            case 0x66:
                                                Serial.print("綠色");
                                                green++;
                                                break;
                                            case 0x77:
                                                Serial.print("藍色");
                                                bulu++;
                                                break;
                                        }
                                        String qrCodeData = "";
                                        String cleanedData = "";
                                        for (int i = 6; i < Data_Length + 6; ++i) {
                                            qrCodeData += (char) Data_OTABuf[i];
                                        }

                                        cleanedData = extractFormula(qrCodeData);
                                        Serial.println(cleanedData);
                                        if (compareWithStringArray(cleanedData, qrCodeData1, 10) == 2) {
                                            Serial.println("不是重复的二维码");
                                            qrCodeData1[shulaing] = cleanedData;
                                            Serial.println(qrCodeData1[shulaing]);
                                            shulaing++;
                                        }
                                        // if (qrCodeData.length() == 8) {  // 假设车牌信息长度为8
                                        //   cleanedData = extractCarPlateInfo(qrCodeData);
                                        //   xx = sumDigits(qrCodeData.c_str());
                                        //   qrCodeDataArray[0] = cleanedData;
                                        // } else if (qrCodeData.length() == 10) {
                                        //   cleanedData = moveUpperCaseChars(qrCodeData.c_str());
                                        //   qrCodeDataArray[2] = cleanedData;
                                        // } else {
                                        //   cleanedData = extractFormula(qrCodeData);
                                        //   qrCodeDataArray[1] = cleanedData;
                                        // }
                                        Serial.print("Processed QR Code Data: ");
                                        Serial.println(qrCodeData1[shulaing]);
                                        // ... 保存或处理cleanedData ...
                                        numberOfQRCodesReceived++;
                                    }
                                }
                                if (numberOfQRCodesExpected != 0 &&
                                    numberOfQRCodesReceived >= numberOfQRCodesExpected) {
                                    break;
                                }
                            }
                            break;
                    }
                    if (numberOfQRCodesExpected == numberOfQRCodesReceived) {
                        break;
                    }
                }
            }

            BEEP.TurnOn();
            delay(50);
            BEEP.TurnOff();
            DCMotor.TurnRight_Code(40, 30);
            K210_Send(0x02);
            delay(2000);
            numberOfQRCodesExpected = 0;
            numberOfQRCodesReceived = 0;  // 修改此行，重置已接收的二维码数量
            break;
        }
    }
    // if (zt == 0) {
    //   if (red < bulu && red < green) {
    //     zt = 1;  // 如果 red 最小，则设置 zt 为 1
    //   } else if (bulu < red && bulu < green) {
    //     zt = 2;  // 如果 blue 最小，则设置 zt 为 2
    //   } else {
    //     zt = 3;  // 否则设置 zt 为 3
    //   }
    // }
    // Serial.print("第一个二维码");
    // Serial.println(xx);
    // Serial.println(qrCodeDataArray[0]);
    // Serial.print("第二个二维码");
    // Serial.println(qrCodeDataArray[1]);
    // Serial.print("第三个二维码");
    // Serial.println(qrCodeDataArray[2]);
    // 关闭二维码识别
    K210_Send(0x02);
    DCMotor.TurnLeft_Code(40, 150);
}


void QR72023(void) {

    //
    static String clh[10];
    Serial.println(qrCodeData1[0]);
    Serial.println(qrCodeData1[1]);

    // 提取字符串中的字母 'A' 或 'B' 并存储到数组中
    int index1 = QR72023qtiqu(qrCodeData1[0]);
    int index2 = QR72023qtiqu(qrCodeData1[1]);
    Serial.println(index1);
    Serial.println(index2);
    // 如果提取成功，则存储提取结果到数组中
    //A<A1B2C3D4E5F6>    =>     A1B2C3D4E5F6
    //B<A1B2C3D4E5F6>    =>     A1B2C3D4E5F6
    //tiqu_72023()
    if (index1 != -1)
        clh[index1] = tiqu_72023(qrCodeData1[0]);
    if (index2 != -1)
        clh[index2] = tiqu_72023(qrCodeData1[1]);

    //A1B2C3D4E5F6    =>         C36D17
    // 打印数组的第二个和第三个元素
    Serial.println(clh[1]);
    Serial.println(clh[2]);

    clh[3] = chuli20237(clh[1], clh[2]);
    Serial.println(clh[3]);


    setGlobalVariable(clh[3]);
}

void setGlobalVariable(String str) {
    // 确保字符串长度为6
    if (str.length() != 6) {
        Serial.println("Error: Input string must be 6 characters long.");
        return;
    }
    Serial.println(str.substring(0, 2));
    Serial.println(str.substring(2, 4));
    Serial.println(str.substring(4, 6));
    char* endptr;
    // 设置全局变量
    wuxiancd2023[0] = strtol(str.substring(0, 2).c_str(), &endptr, 16);  // 设置 wuxiancd2023[0]
    wuxiancd2023[1] = strtol(str.substring(2, 4).c_str(), &endptr, 16);  // 设置 wuxiancd2023[1]
    wuxiancd2023[2] = strtol(str.substring(4, 6).c_str(), &endptr, 16);  // 设置 wuxiancd2023[2]

    Serial.println(wuxiancd2023[0]);
    Serial.println(wuxiancd2023[1]);
    Serial.println(wuxiancd2023[2]);
}


void setGlobalVariable0328(String str) {
    // 确保字符串长度为6
    if (str.length() != 12) {
        Serial.println("Error: Input string must be 6 characters long.");
        return;
    }
    Serial.println(str.substring(0, 2));
    Serial.println(str.substring(2, 4));
    Serial.println(str.substring(4, 6));
    char* endptr;
    // 设置全局变量
    wuxiancd2023[0] = strtol(str.substring(0, 2).c_str(), &endptr, 16);    // 设置 wuxiancd2023[0]
    wuxiancd2023[1] = strtol(str.substring(2, 4).c_str(), &endptr, 16);    // 设置 wuxiancd2023[1]
    wuxiancd2023[2] = strtol(str.substring(4, 6).c_str(), &endptr, 16);    // 设置 wuxiancd2023[2]
    wuxiancd2023[3] = strtol(str.substring(6, 8).c_str(), &endptr, 16);    // 设置 wuxiancd2023[2]
    wuxiancd2023[4] = strtol(str.substring(8, 10).c_str(), &endptr, 16);   // 设置 wuxiancd2023[2]
    wuxiancd2023[5] = strtol(str.substring(10, 12).c_str(), &endptr, 16);  // 设置 wuxiancd2023[2]

    Serial.println(wuxiancd2023[0]);
    Serial.println(wuxiancd2023[1]);
    Serial.println(wuxiancd2023[2]);
    Serial.println(wuxiancd2023[3]);
    Serial.println(wuxiancd2023[4]);
    Serial.println(wuxiancd2023[5]);
}


String chuli20237(String str1, String str2) {
    String result = "";

    // 找到字符串1中与字符串2不同的部分
    for (int i = 0; i < 121; ++i) {
        char currentChar = str1.charAt(i);
        char currentChar2 = str2.charAt(i);
        if (currentChar != currentChar2) {
            result += currentChar;
        }
    }

    // 找到字符串2中与字符串1不同的部分
    for (int i = 0; i < 12; ++i) {
        char currentChar = str2.charAt(i);
        char currentChar2 = str1.charAt(i);
        if (currentChar != currentChar2) {
            result += currentChar;
        }
    }

    return result;
}


int QR72023qtiqu(String rawData) {
    char AORB = rawData.charAt(0);
    Serial.println(AORB);
    if (AORB == 'A') {
        return 1;
    }
    if (AORB == 'B') {
        return 2;
    }
}

String tiqu_72023(String rawData) {
    int sta = 0, enda = 0;
    bool inBrackets = false;  // 用来跟踪是否在尖括号内部
    for (int i = 0; i < rawData.length(); ++i) {
        char ch = rawData.charAt(i);
        if (ch == '<') {
            sta = i;
            inBrackets = true;
        }
        // 如果遇到 '>'，离开尖括号内部，并将 f_index 归零
        if (ch == '>' && inBrackets) {
            enda = i;
            break;
        }
    }
    String fanhui = "";
    for (int staa = sta; staa < enda; ++staa) {
        char ch = rawData.charAt(staa);
        // 检查字符是否为大写字母或数字
        if ((ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9')) {
            fanhui += ch;
        }
    }
    return fanhui;
}


void Road_QR_Test2(void) {
    int numberOfQRCodesExpected = 0;  // 预期接收的二维码数量
    int numberOfQRCodesReceived = 0;  // 已接收的二维码数量
    int i = 0, red = 0, bulu = 0, green = 0;;  // 超时计数器

    DCMotor.TurnLeft_Code(40, 120);
    // 开启二维码识别

    for (int i = 0; i < 15; i++) {
        K210_Send(0x01);
        delay(1000);
        while (true) {
            // 检查是否有数据可读
            if (ExtSRAMInterface.ExMem_Read(0x6038) != 0x00) {
                // 读取数据类型、标志和长度
                uint8_t Data_Type = ExtSRAMInterface.ExMem_Read(0x603A);
                uint8_t Data_Flag = ExtSRAMInterface.ExMem_Read(0x603B);
                uint8_t Data_Length = ExtSRAMInterface.ExMem_Read(0x603C);
                uint8_t dataLength = ExtSRAMInterface.ExMem_Read(0x603C) + 6;
                delay(100);
                ExtSRAMInterface.ExMem_Read_Bytes(0x6038, Data_OTABuf, dataLength);

                if ((Data_OTABuf[0] == 0x55) && (Data_OTABuf[1] == 0x02)) {
                    switch (Data_OTABuf[3]) {
                        case 0x00:  // 接收到二维码数量
                            numberOfQRCodesExpected = Data_OTABuf[4];
                            Serial.print("Number of QR Codes: ");
                            Serial.println(numberOfQRCodesExpected);
                            while (true) {
                                if (ExtSRAMInterface.ExMem_Read(0x6038) != 0x00) {
                                    // 读取数据类型、标志和长度
                                    uint8_t Data_Type = ExtSRAMInterface.ExMem_Read(0x603A);
                                    uint8_t Data_Flag = ExtSRAMInterface.ExMem_Read(0x603B);
                                    uint8_t Data_Length = ExtSRAMInterface.ExMem_Read(0x603C);
                                    uint8_t dataLength = ExtSRAMInterface.ExMem_Read(0x603C) + 6;
                                    ExtSRAMInterface.ExMem_Read_Bytes(0x6038, Data_OTABuf, dataLength);
                                    delay(100);
                                    if ((Data_OTABuf[0] == 0x55) && (Data_OTABuf[1] == 0x02) &&
                                        (Data_OTABuf[3] == 0x01)) {
                                        String qrCodeData = "";
                                        for (int i = 6; i < Data_Length + 6; ++i) {
                                            qrCodeData += (char) Data_OTABuf[i];
                                        }
                                        String cleanedData = "";
                                        switch (Data_OTABuf[5]) {
                                            case 0x55:
                                                Serial.print("紅色");
                                                qrCodeRedData2[0] = extractFormula(qrCodeData.c_str());
                                                red++;
                                                break;
                                            case 0x66:
                                                Serial.print("綠色");
                                                qrCodeGreenData2[0] = extractFormula(qrCodeData.c_str());
                                                green++;
                                                break;
                                            case 0x77:
                                                Serial.print("藍色");
                                                qrCodeBlueData2[0] = extractFormula(qrCodeData.c_str());
                                                bulu++;
                                                break;
                                        }
                                        if (qrCodeData.length() == 8) {  // 假设车牌信息长度为8
                                            cleanedData = extractCarPlateInfo(qrCodeData);
                                            xx = sumDigits(qrCodeData.c_str());
                                            qrCodeDataArray[0] = cleanedData;
                                        } else if (qrCodeData.length() == 10) {
                                            cleanedData = extractFormula(qrCodeData.c_str());
                                            qrCodeDataArray[2] = cleanedData;
                                        } else {
                                            cleanedData = extractFormula(qrCodeData.c_str());
                                            // cleanedData = extractFormula(qrCodeData);
                                            qrCodeDataArray[1] = cleanedData;
                                        }
                                        Serial.print("Processed QR Code Data: ");
                                        Serial.println(cleanedData);
                                        // ... 保存或处理cleanedData ...
                                        numberOfQRCodesReceived++;
                                    }
                                }
                                if (numberOfQRCodesExpected != 0 &&
                                    numberOfQRCodesReceived >= numberOfQRCodesExpected) {
                                    break;
                                }
                            }
                            break;
                    }
                    if (numberOfQRCodesExpected == numberOfQRCodesReceived) {
                        break;
                    }
                }
            }
            BEEP.TurnOn();
            delay(50);
            BEEP.TurnOff();
            DCMotor.TurnRight_Code(60, 30);
            K210_Send(0x02);
            delay(2000);
            numberOfQRCodesExpected = 0;
            numberOfQRCodesReceived = 0;  // 修改此行，重置已接收的二维码数量
            break;
        }
    }
    if (zt == 0) {
        if (red < bulu && red < green) {
            zt = 1;  // 如果 red 最小，则设置 zt 为 1
        } else if (bulu < red && bulu < green) {
            zt = 2;  // 如果 blue 最小，则设置 zt 为 2
        } else {
            zt = 3;  // 否则设置 zt 为 3
        }
    }
    Serial.println("第一个二维码");
    // Serial.println(xx);
    Serial.println(qrCodeRedData2[0]);
    Serial.println("第二个二维码");
    Serial.println(qrCodeGreenData2[0]);
    Serial.println("第三个二维码");
    Serial.println(qrCodeBlueData2[0]);


    tiqu(qrCodeRedData2[0]);
    tiqu2(qrCodeGreenData2[0]);
    // 关闭二维码识别
    K210_Send(0x02);
    DCMotor.TurnLeft_Code(60, 150);
}

String filterString(String input, int start, int end) {
    if (start < 0 || end >= input.length() || start > end) {
        // 如果开始位置小于0，结束位置大于等于字符串长度，或者开始位置大于结束位置，返回空字符串
        return "";
    }

    // 截取开始位置之前的部分
    String filteredString = input.substring(0, start);

    // 截取结束位置之后的部分
    filteredString += input.substring(end + 1);

    return filteredString;
}


String tiqu(String rawData) {
    int f[2] = {0};
    char w[2] = {0};
    int f_index = 0;  // 使用 f_index 来跟踪 f 数组的索引
    int sta = 0, enda = 0;
    bool inBrackets = false;  // 用来跟踪是否在尖括号内部
    for (int i = 0; i < rawData.length(); ++i) {
        char ch = rawData.charAt(i);
        if (ch == '<') {
            sta = i;
            inBrackets = true;
        }
        // 如果遇到 '>'，离开尖括号内部，并将 f_index 归零
        if (ch == '>' && inBrackets) {
            enda = i;
            break;
        }
    }
    for (int staa = sta; staa < enda; ++staa) {
        char ch = rawData.charAt(staa);
        // 如果在尖括号内部并且是数字，存储到 f 数组
        if (ch >= '0' && ch <= '9') {
            if (f_index == 0) {
                f[0] = ch - '0';
            } else if (f_index == 1) {
                f[1] = ch - '0';
                // 已经存储了两个数字，提前退出循环
                break;
            }
            f_index++;
        }
    }
    f_index = 0;
    String rawDataxg = filterString(rawData, sta, enda);

    Serial.println(rawDataxg);

    for (int i = 0; i < rawDataxg.length(); ++i) {
        char ch = rawData.charAt(i);
        if (ch >= '0' && ch <= '9') {
            if (f_index == 0) {
                w[0] = ch;
            } else if (f_index == 1) {
                w[1] = ch;
                // 已经存储了两个数字，提前退出循环
                break;
            }
            f_index++;
        }
    }
    wuxiancd[f[0]] = w[0];
    wuxiancd[f[1]] = w[1];
    // 打印提取的数字
    Serial.println(f[0]);
    Serial.println(f[1]);
    Serial.println(w[0]);
    Serial.println(w[1]);
    // 返回空字符串以清空上一次的值
    return "";
}

String tiqu2(String rawData) {
    int f[2] = {0};
    char w[2] = {0};
    int f_index = 0;  // 使用 f_index 来跟踪 f 数组的索引
    int sta = 0, enda = 0;
    bool inBrackets = false;  // 用来跟踪是否在尖括号内部
    for (int i = 0; i < rawData.length(); ++i) {
        char ch = rawData.charAt(i);
        if (ch == '<') {
            sta = i;
            inBrackets = true;
        }
        // 如果遇到 '>'，离开尖括号内部，并将 f_index 归零
        if (ch == '>' && inBrackets) {
            enda = i;
            break;
        }
    }
    for (int staa = sta; staa < enda; ++staa) {
        char ch = rawData.charAt(staa);
        // 如果在尖括号内部并且是数字，存储到 f 数组
        if (ch >= '0' && ch <= '9') {
            if (f_index == 0) {
                f[0] = ch - '0';
            } else if (f_index == 1) {
                f[1] = ch - '0';
                // 已经存储了两个数字，提前退出循环
                break;
            }
            f_index++;
        }
    }
    f_index = 0;
    String rawDataxg = filterString(rawData, sta, enda);

    Serial.println(rawDataxg);

    for (int i = 0; i < rawDataxg.length(); ++i) {
        char ch = rawData.charAt(i);
        if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z')) {
            if (f_index == 0) {
                w[0] = ch;
            } else if (f_index == 1) {
                w[1] = ch;
                // 已经存储了两个数字，提前退出循环
                break;
            }
            f_index++;
        }
    }
    wuxiancd[f[0]] = w[0];
    wuxiancd[f[1]] = w[1];
    // 打印提取的数字
    Serial.println(f[0]);
    Serial.println(f[1]);
    Serial.println(w[0]);
    Serial.println(w[1]);
    // 返回空字符串以清空上一次的值
    return "";
}
