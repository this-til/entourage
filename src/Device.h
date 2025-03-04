//
// Created by til on 2025/2/27.
//

#ifndef ENTOURAGE_CLION_DEVICE_H
#define ENTOURAGE_CLION_DEVICE_H

#include "def.h"

/***
 * 天气
 */
enum Weather {
    /***
     * 大风
     */
    GALE = 0x00,

    /***
     * 多云
     */
    CLOUDY = 0x01,

    /***
     * 晴
     */
    SUNNY = 0x02,

    /***
     * 小雪
     */
    LIGHT_SNOW = 0x03,

    /***
     * 小雨
     */
    SPIT = 0x04,

    /***
     * 阴天
     */
    OVERCAST_SKY = 0x05
};

/***
 * 文本编码格式
 */
enum TextEncodingFormat {
    GB2312 = 0x00,
    GBK = 0x01,
    BIG5 = 0x02,
    Unicode = 0x03,
};

enum TimingMode {
    TIMING_OFF = 0x00,
    TIMING_ON = 0x01,
    CLOCK_RESET = 0x02,
};

enum TrafficLightModel {
    RED = 0x01,
    GREEN = 0x02,
    YELLOW = 0x03,
};

enum K210Color {
    K_NONE = 0x00,
    K_WHITE = 0x01,
    K_BLACK = 0x02,
    K_RED = 0x03,
    K_GREEN = 0x04,
    K_YELLOW = 0x05,
};

void logObj(Weather weather);

void logObj(TextEncodingFormat textEncodingFormat);

void logObj(TimingMode timingMode);

void logObj(TrafficLightModel trafficLightModel);

void logObj(K210Color k210Color);

class DeviceBase {
public:
    explicit DeviceBase(uint8_t id);

    void send(uint8_t* buf);

    bool check(const uint8_t* buf, const uint8_t* serviceId, uint8_t serviceLen);

    bool awaitReturn(uint8_t buf[], const uint8_t* serviceId, uint8_t serviceLen);

    bool check(const uint8_t* buf, uint8_t serviceId);

    bool awaitReturn(uint8_t buf[], uint8_t serviceId);

protected:
    uint8_t id;

    uint16_t sendBus;
    uint8_t sendLen;
    bool sendVerify;
    int32_t sendCooling_ms;

    uint16_t readBus;
    uint8_t readLen;
    bool readVerify;
    bool readTailIntegrity;
    unsigned long readOutTime_ms;

    bool debug;
};

/***
 * 报警台
 */
class AlarmDesk : DeviceBase {
public:
    explicit AlarmDesk(uint8_t id);

    /***
     * 使用红外线开启报警台，默认开启码是6位
     */
    void openByInfrared(uint8_t* openCode);

    /***
     * 获取救援位置
     */
    uint8_t getRescuePosition();

    /***
     * debug
     * 设置开启码
     * @param data[] len=6
     */
    void setOpenCode(uint8_t data[]);

};

/***
 * 智能道闸标控制
 */
class BarrierGate : DeviceBase {
public:
    explicit BarrierGate(uint8_t id);

    /***
     * 设置门闸
     * @param open id true 开启
     */
    void setGateControl(bool open);

    /***
     * 获取门闸
     * "智能道闸标志物处于关闭状态时请求回传状态，不会回传任何指令。"
     */
    bool getGateControl();
};

/***
 * 公交站
 */
class BusStop : DeviceBase {
public:
    explicit BusStop(uint8_t id);

    /***
     * 播报指定语音命令
     * @param languageId
     * 播报“富强路站”（编号：01）
     * 播报“民主路站”（编号：02）
     * 播报“文明路站”（编号：03）
     * 播报“和谐路站”（编号：04）
     * 播报“爱国路站”（编号：05）
     * 播报“敬业路站”（编号：06）
     * 播报“友善路站”（编号：07）
     *
     */
    void broadcastSpeech(uint8_t languageId);

    /***
     * 播报随机语音命令
     */
    void broadcastRandomSpeech();

    /***
     * 设置RTC起始日期
     * @param yy 年
     * @param MM 月
     * @param dd 天
     */
    void setRtcStartDate(uint8_t yy, uint8_t MM, uint8_t dd);

    /***
     * 设置RTC起始时间
     * @param HH 时
     * @param mm 分
     * @param ss 秒
     */
    void setRtcStartTime(uint8_t HH, uint8_t mm, uint8_t ss);

    /***
    * 查询RTC当前日期
    */
    bool getRtcDate(uint8_t* yy, uint8_t* MM, uint8_t* dd);

    /***
     * 查询RTC当前时间
     */
    bool getRtcTime(uint8_t* HH, uint8_t* mm, uint8_t* ss);

    /***
     * 设置天气数据与温度数据
     */
    void setWeatherTemperature(Weather weather, uint8_t temperature);

    /***
     * 请求回传天气数据与温度数据
     */
    bool getWeatherTemperature(Weather* weather, uint8_t* temperature);

    /***
     * 开始合成语言
     */
    void startSynthesizingLanguage(const uint8_t* data, uint16_t len, TextEncodingFormat textEncodingFormat);

    /***
     * 停止合成语音
     */
    void stopSynthesizingLanguage();

    /***
       * 暂停合成语言
       */
    void pauseSynthesizingLanguage();

    /***
       * 恢复合成语言
       */
    void recoverSynthesizingLanguage();

};


/***
 *  智能显示标志物控制
 */
class Monitor : DeviceBase {
public:
    explicit Monitor(int id);

    /***
     * 设置数码管显示指定数据
     * @param pos 第几个数码管 1:第一排 2:第二排
     * @param value[] 值 0~9 长度6
     */
    void setDisplayData(uint8_t pos, const uint8_t value[]);

    /***
     * 设置第一排数码管显示计时模式
     */
    void setTimingMode(TimingMode timingMode);

    /***
     * 智能显示标志物第二排数码管显示距离模式，第二位和第三位副指令中“X”为要显示的距离值，单位为毫米，显示格式为十进制。
     * @param distance_mm 距离单位毫米
     */
    void setDistance(uint16_t distance_mm);

};

/***
 * 智能立体车库
 */
class Carport : DeviceBase {

public:

    explicit Carport(uint8_t id);

    /***
    * 将车库移动到指定层
    * @param level 指定层
    */
    void moveToLevel(uint8_t level);


    /***
    * 获取车库层数
     * @param level 将层数输出到该指针
    */
    bool getLevel(uint8_t * level);

    /***
    * 获取车库前后侧红外状态
    * @param ventral out 前侧
    * @param rearSide out 后侧
    */
    void getInfraredState(bool* ventral, bool* rearSide);


};

/***
 * 智能交通信号灯
 */
class TrafficLight : DeviceBase {
public:
    explicit TrafficLight(uint8_t id);

    /***
     * 请求进入识别模式
     */
    void requestToEnterRecognitionMode();

    /***
     * 请求确认识别结果
     */
    void requestConfirmationOfIdentificationResults(TrafficLightModel trafficLightModel);

};

/***
 * 路灯
 */
class StreetLamp : DeviceBase {
public:
    explicit StreetLamp(uint8_t id);

    //TODO 无线通信
};

/***
 * 信息显示物(广告牌)
 */
class AdvertisingBoard : DeviceBase {

};

struct QrMessage {
    bool efficient;

    K210Color qrColor = K_WHITE;

    /***
     * 二维码信息容器
     */
    uint8_t* message;

    uint8_t messageMaxLen = 0;

    uint8_t messageLen = 0;
};

class K230 : DeviceBase {
public:


    explicit K230(uint8_t id);

    /***
    * 设置摄像头舵机角度
    * @param angle 角度 舵机角度，范围-80至+40，0度垂直于车身
    */
    void setCameraSteeringGearAngle(int8_t angle);

    /***
    * 设置寻迹是否开启
    */
    void setTrackModel(bool open);

    /***
     * 获取寻迹标志位
     * 高八位为靠近中心点
     * 该函数需要开启寻迹
     * @return
     */
    bool getTrackFlagBit(uint8_t* trackFlagBit);

    /***
     *
     * @param count
     * @param qrMessageArray
     * @param len
     */
    bool qrRecognize(uint8_t* count, QrMessage* qrMessageArray, uint8_t maxLen);

    /***
     * 检测红绿灯
     * @param color
     */
    bool trafficLightRecognize(K210Color* k210Color);

    /***
     * 多从重复以获取精准的值
     * @param k210Color
     * @param consecutiveEqualDegree 相同值连续出现的次数
     * @param maxRetry 最大重试次数
     * @return
     */
    bool trafficLightRecognize_rigorous(K210Color* k210Color, uint16_t consecutiveEqualDegree = 3, uint16_t maxRetry = 48);

    /***
     * 测试连接用的
     * @return
     */
    bool ping();

    //void loop();

private:
    bool receiveTrack;
};

/***
 * 你的小车车
 * 一些和车有关的，杂七杂八的就扔在这里
 */
class Car {
public:
    Car();

/***
 * 直行到下一个路口
 * @param carSpeed
 */
    void straightLine();

    void turnLeft(bool trimCar = true);

    void turnRight(bool trimCar = true);

    /***
     * 微调车姿态
     */
    void trimCar();

    bool acceptTrackFlag(uint8_t* outTrackFlag, uint32_t* outBitCount, float* outOffset);

    int16_t straightLineSpeed;
    int16_t turnLeftSpeed;
    int16_t turnRightSpeed;
    int16_t straightLineKpSpeed;
    int16_t trimAttitudeKpSpeed;

    unsigned long trimOutTime_ms;
    unsigned long outTime_ms;


};

/***
 * 报警台
 */
extern AlarmDesk alarmDeskA;

/***
 * 智能道闸标控制
 */
extern BarrierGate barrierGateA;

/***
 * 公交站
 */
extern BusStop busStopA;

/***
 *  智能显示标志物控制
 */
extern Monitor monitorA;

/***
 * 智能立体车库
 */
extern Carport carportA;
extern Carport carportB;

/***
 * 智能交通信号灯
 */
extern TrafficLight trafficLightA;
extern TrafficLight trafficLightB;
extern TrafficLight trafficLightC;
extern TrafficLight trafficLightD;

/***
 * 路灯
 */
extern StreetLamp streetLampA;

/***
 * 摄像头
 */
extern K230 k230;
/***
 * 你的小车车
 */
extern Car car;


#endif //ENTOURAGE_CLION_DEVICE_H
