#include "Infrare.h"

//================立体标志物================================
void TEXT_display(const char* data) {
    char Text_buf[6] = {0xFF, 0x20};
    // 将 data 中的字符复制到 Text_buf 中
    for (int i = 0; i < 4; i++) {
        Text_buf[i + 2] = data[i];
    }
    // 调用红外传输函数
    Infrare.Transmition(Text_buf, 6);
}

void TEXT_display2(const char* data) {
    char Text_buf2[6] = {0xFF, 0x10};
    // 将 data 中的字符复制到 Text_buf2 中
    for (int i = 0; i < 4; i++) {
        Text_buf2[i + 2] = data[i];
    }
    // 调用红外传输函数
    Infrare.Transmition(Text_buf2, 6);
}

void TEXT_display3(char data) {
    char Text_buf3[6] = {0xFF, 0x15};
    // 将 data 中的字符复制到 Text_buf2 中
    Text_buf3[2] = data;
    // 调用红外传输函数
    Infrare.Transmition(Text_buf3, 6);
}

//车牌坐标
//ABC356
//BEF589
void TEXT_display4(const char* data, const char* wz) {
    char Text_buf[6] = {0xFF, 0x20};
    char Text_buf2[6] = {0xFF, 0x10};
    // 将 data 中的字符复制到 Text_buf2 中

    Text_buf[2] = data[0];
    Text_buf[3] = data[1];
    Text_buf[4] = data[2];
    Text_buf[5] = data[3];

    Text_buf2[2] = data[4];
    Text_buf2[3] = data[5];
    Text_buf2[4] = wz[0];
    Text_buf2[5] = wz[1];


    // 调用红外传输函数
    Infrare.Transmition(Text_buf, 6);
    delay(1000);
    Infrare.Transmition(Text_buf2, 6);
    delay(1000);
}


//================报警标志物================================
char baojingtai_open[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

void BJT_OPEN(char a, char b, char c, char d, char e, char f) {
    baojingtai_open[0] = a;
    baojingtai_open[1] = b;
    baojingtai_open[2] = c;
    baojingtai_open[3] = d;
    baojingtai_open[4] = e;
    baojingtai_open[5] = f;
    Infrare.Transmition(baojingtai_open, 6);
}

void LTBZW(String str) {
    // 分别调用 TEXT_display 显示前四位和 TEXT_display2 显示后四位及其坐标信息
    TEXT_display(str.substring(0, 4).c_str());
    delay(500);
    TEXT_display2(str.substring(4).c_str());
    delay(500);
}