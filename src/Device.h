//
// Created by til on 2025/2/27.
//

#ifndef ENTOURAGE_CLION_DEVICE_H
#define ENTOURAGE_CLION_DEVICE_H

//哄老母鸡被迫留下来的痔疮代码
#define FOR_THE_KING_HEN true

#define DETECTED_VALUE_CHANGE(name)                                                                                     \
unsigned long startTime = millis();                                                                                     \
uint8_t name##Version = this->name##Version;                                                                            \
while (millis() - startTime < 1000) {                                                                                   \
    if(name##Version != this->name##Version) {                                                                          \
        if ((name) != nullptr) {                                                                                        \
            *(name) = this->name;                                                                                       \
        }                                                                                                               \
        return true;                                                                                                    \
    }                                                                                                                   \
    yield();                                                                                                            \
}                                                                                                                       \
return false;                                                                                                           \

#define TWO_DETECTED_VALUE_CHANGE(name1, name2)                                                                         \
unsigned long startTime = millis();                                                                                     \
uint8_t name1##_##name2##Version = this->name1##_##name2##Version;                                                      \
while (millis() - startTime < 1000) {                                                                                   \
    if(name1##_##name2##Version != this->name1##_##name2##Version) {                                                    \
        if ((name1) != nullptr) {                                                                                       \
            *(name1) = this->name1;                                                                                     \
        }                                                                                                               \
        if ((name2) != nullptr) {                                                                                       \
            *(name2) = this->name2;                                                                                     \
        }                                                                                                               \
        return true;                                                                                                    \
    }                                                                                                                   \
    yield();                                                                                                            \
}                                                                                                                       \
return false;                                                                                                           \

#define THERE_DETECTED_VALUE_CHANGE(name1, name2, name3)                                                                \
unsigned long startTime = millis();                                                                                     \
    uint8_t name1##_##name2##_##name3##Version = this->name1##_##name2##_##name3##Version;                              \
    while (millis() - startTime < 1000) {                                                                               \
    if(name1##_##name2##_##name3##Version != this->name1##_##name2##_##name3##Version) {                                \
        if ((name1) != nullptr) {                                                                                       \
            *(name1) = this->name1;                                                                                     \
        }                                                                                                               \
        if ((name2) != nullptr) {                                                                                       \
            *(name2) = this->name2;                                                                                     \
        }                                                                                                               \
        if ((name3) != nullptr) {                                                                                       \
            *(name3) = this->name3;                                                                                     \
        }                                                                                                               \
        return true;                                                                                                    \
    }                                                                                                                   \
    yield();                                                                                                            \
}                                                                                                                       \
return false;                                                                                                           \


#define UPDATE_VALUE(conditions, name, value)                                                                           \
if (conditions) {                                                                                                       \
    (name)=(value);                                                                                                     \
    name##Version++;                                                                                                    \
}                                                                                                                       \

#define TWO_UPDATE_VALUE(conditions, name1, value1, name2, value2)                                                      \
if (conditions) {                                                                                                       \
    (name1)=(value1);                                                                                                   \
    (name2)=(value2);                                                                                                   \
    name1##_##name2##Version++;                                                                                         \
}                                                                                                                       \


#define THERE_UPDATE_VALUE(conditions, name1, value1, name2, value2, name3, value3)                                     \
if (conditions) {                                                                                                       \
    (name1)=(value1);                                                                                                   \
    (name2)=(value2);                                                                                                   \
    (name3)=(value3);                                                                                                   \
    name1##_##name2##_##name3##Version++;                                                                               \
}                                                                                                                       \


#define VALUE_VERSION(type, name)                                                                                       \
type name;                                                                                                              \
uint8_t name##Version;                                                                                                  \

#define TWO_VALUE_VERSION(type1, name1, type2, name2)                                                                   \
type1 name1;                                                                                                            \
type2 name2;                                                                                                            \
uint8_t name1##_##name2##Version;                                                                                       \

#define THERE_VALUE_VERSION(type1, name1, type2, name2, type3, name3)                                                   \
type1 name1;                                                                                                            \
type2 name2;                                                                                                            \
type3 name3;                                                                                                            \
uint8_t name1##_##name2##_##name3##Version;                                                                             \


#include "def.h"
#include "Enum.h"
#include "Cache.h"


void logObj(enum Weather weather);

void logObj(enum TextEncodingFormat textEncodingFormat);

void logObj(enum TimingMode timingMode);

void logObj(enum TrafficLightModel trafficLightModel);

void logObj(enum K210Color k210Color);

TrafficLightModel k230ColorToTrafficLightModel(K210Color k210Color);

void deviceSetup();

class MessageBus {
public:
    uint8_t computeVerify(const uint8_t* buf, uint8_t len);

    void addVerify(uint8_t* buf, uint8_t len);

    void send(uint8_t* buf, uint8_t len, uint16_t bus = 0x6008, bool addVerify = true, unsigned long sendCoolingMs = 300);

    bool sendAndWait(uint8_t* buf, uint8_t len, const uint8_t* returnCount, uint16_t bus = 0x6008, bool addVerify = true, unsigned long sendCoolingMs = 300, unsigned long maxWaitingTimeMs = 5000);

    bool check(const uint8_t* buf, uint8_t len, uint8_t id, const uint8_t* serviceId, uint8_t serviceLen, bool verify = true);

    bool check(const uint8_t* buf, uint8_t len, uint8_t id, uint8_t serviceId, bool verify = true);

    bool awaitReturn(uint8_t* buf, uint8_t len, uint8_t id, const uint8_t* serviceId, uint8_t serviceLen, uint16_t bus = 0x6008, bool verify = true, unsigned long readOutTimeMs = 1000);

    bool awaitReturn(uint8_t* buf, uint8_t len, uint8_t id, uint8_t serviceId, uint16_t bus = 0x6008, bool verify = true, unsigned long readOutTimeMs = 1000);

protected:
    unsigned long lastSentTime = 0;
};

class DeviceBase {
public:
    explicit DeviceBase(uint8_t id);

    /***
     * 当受到zigbee消息，并且帧头id==设备id
     * @param buf
     */
    virtual void onReceiveZigbeeMessage(uint8_t buf[]);

    uint8_t id;
};

/***
 * 报警台
 */
class AlarmDesk : public DeviceBase {
public:

    VALUE_VERSION(uint8_t, rescuePosition)

    void onReceiveZigbeeMessage(uint8_t* buf) override;

    explicit AlarmDesk(uint8_t id);

    /***
     * 使用红外线开启报警台，默认开启码是6位
     */
    void openByInfrared(uint8_t* openCode);

    /***
     * 获取救援位置
     */
    bool getRescuePosition(uint8_t* rescuePosition);

    void setOpenCode(uint8_t* data);
};

/***
 * 智能道闸标控制
 */
class BarrierGate : public DeviceBase {
public:
    VALUE_VERSION(bool, gateControl)


    explicit BarrierGate(uint8_t id);

    /***
     * 设置门闸
     * @param open id true 开启
     */
    void setGateControl(bool open);

    /***
     * debug
     * 设置初始角度
     *
     * @param isUp true 闸门上升
     */
    void setInitialPos(bool isUp);

    /***
     * 发送车牌
     * @param data len = 6
     * @return
     */
    void setLicensePlateData(uint8_t* data);

    /***
     * 获取门闸
     * "智能道闸标志物处于关闭状态时请求回传状态，不会回传任何指令。"
     */
    bool getGateControl();

};

/***
 * 公交站
 */
class BusStop : public DeviceBase {
public:
    THERE_VALUE_VERSION(uint8_t, yy, uint8_t, MM, uint8_t, dd)
    THERE_VALUE_VERSION(uint8_t, HH, uint8_t, mm, uint8_t, ss)
    TWO_VALUE_VERSION(Weather, weather, uint8_t, temperature)

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

    void onReceiveZigbeeMessage(uint8_t* buf) override;

};


/***
 *  智能显示标志物控制
 */
class Monitor : public DeviceBase {
public:
    explicit Monitor(int id);

    /***
     * 设置数码管显示指定数据
     * @param pos 第几排数码管 1:第一排 2:第二排
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
class Carport : public DeviceBase {

public:

    VALUE_VERSION(uint8_t, level)
    TWO_VALUE_VERSION(bool, ventral, bool, rearSide)

    explicit Carport(uint8_t id);

    /***
    * 将车库移动到指定层
    * @param level 指定层 [1,4]
    */
    void moveToLevel(uint8_t level);


    /***
    * 获取车库层数
     * @param level 将层数输出到该指针
    */
    bool getLevel(uint8_t* level);

    /***
    * 获取车库前后侧红外状态
    * @param ventral out 前侧 (true 表示有东西遮挡)
    * @param rearSide out 后侧
    */
    bool getInfraredState(bool* ventral, bool* rearSide);

    void onReceiveZigbeeMessage(uint8_t* buf) override;


};

/***
 * 智能交通信号灯
 */
class TrafficLight : public DeviceBase {
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
class StreetLamp : public DeviceBase {
public:
    explicit StreetLamp(uint8_t id);

    //TODO 无线通信

    void setLightSourceIntensity(LightSourceIntensity lightSourceIntensity);
};


/***
 * 无线充电
 */
class WirelessCharging : public DeviceBase {
public:
    explicit WirelessCharging(uint8_t id);

    /***
     * debug
     * 开启无线充电模式
     * 之后调用openWirelessCharging
     * @param open
     */
    void setOpenWirelessCharging(bool open);

    /***
     * 开启无线充电
     * @param openCode len = 3
     */
    void openWirelessCharging(uint8_t* openCode);


    /***
     * debug
     * @param openCode len = 3
     */
    void setWirelessChargingOpenCode(uint8_t* openCode);
};

/***
 * 信息显示物
 */
class InformationDisplay : public DeviceBase {
public:
    explicit InformationDisplay(uint8_t id);

    /***
     * 显示指定图片
     * @return
     */
    void showSpecifiedPicture(uint8_t id);

    /***
     * 设置翻页模式
     */
    void setThePageTurningMode(PageTurningMode pageTurningMode);

    /***
     * 显示车牌
     * @param licensePlate len = 6
     */
    void showLicensePlate(uint8_t* licensePlate);

    /***
     * 设置显示模式
     * @param timingMode
     */
    void setTimingMode(InformationDisplayTimingMode timingMode);

    /***
     * 显示数据
     * @param data len = 3 (十进制仅两位有效，但还是给3长度)
     */
    void showData(uint8_t* data, BaseFormat baseFormat);

    /***
     * 显示交通标志
     * @param trafficSigns
     */
    void showTrafficSigns(TrafficSign trafficSigns);

};

class ETC : public DeviceBase {

public:
    explicit ETC(uint8_t id);

    /***
    * debug
    * 设置初始角度
    *
    * @param true 闸门上升
    */
    void setInitialPos(bool lUp, bool rUp);

    //TODO 回传接收
};


class StereoscopicDisplay : public DeviceBase {
public:
    explicit StereoscopicDisplay(uint8_t id);


    /***
     *
     * @param licensePlate len = 6
     * @param x 横坐标
     * @param y 纵坐标
     */
    void showLicensePlate(uint8_t* licensePlate, uint8_t x, uint8_t y);

    /***
     * 显示距离
     * @param value 距离
     */
    void showDistance(uint8_t value);

    /***
     * 显示"图形"
     */
    void showGraphText(Graph graph);

    /***
     * //设置显示颜色
     * 显示“颜色”
     */
    void showColorText(StereoscopicDisplayColor stereoscopicDisplayColor);

    /***
     * 交通警示牌
     */
    void showTrafficWarningSigns(TrafficWarningSigns trafficWarningSigns);

    /***
     * 显示交通标识牌
     */
    void showTrafficSign(TrafficSign trafficSign);

    /***
     * 显示默认信息
     */
    void showDefaultInformation();

    /***
     * 设置文字颜色
     */
    void setTextColor(uint8_t r, uint8_t g, uint8_t b);

    /***
     * 显示文字 GBK
     */
    void showText(const uint16_t* gbk, uint8_t len);

    void showTextByZigBee(const uint16_t* gbk, uint8_t len);

};

class UltrasonicDevice {
public:

/***
 * 超声波测距
 * @return 测得距离
 */
    double ranging(int sys = CM, int rangeFrequency = 5, int wait = 30);

/***
 * 通过超声波测距控制测距
 * 该操作会暂停小车
 * @param carSeep 车速
 * @param distance 距离
 * @param errorMargin 允许的误差
 * @param controlTime_ms 调整时间，如果超过时间无论有没有到指定距离都将退出函数
 * @return 如果true表示调整到指定距离
 */
    void adjustDistance(uint8_t carSeep, uint8_t targetDistance, double errorMargin = 0.2, unsigned long controlTime_ms = 2000, int wait = 30);
};

struct QrMessage {

    /***
 * 二维码信息容器
 */
    uint8_t* message = nullptr;

    uint8_t messageMaxLen = 0;

    uint8_t messageLen = 0;

    bool efficient{};

    K210Color qrColor = K_WHITE;
};


class K230 : DeviceBase {
public:

    explicit K230(uint8_t id);


    int8_t getCameraSteeringGearAngleCache();

    /***
    * 设置摄像头舵机角度
    * @param angle 角度 舵机角度，范围-80至+40，0度垂直于车身
    */
    void setCameraSteeringGearAngle(int8_t angle);

    /***
    * 设置寻迹是否开启
    */
    void setTrackModel(uint8_t model);

    void setTrackModel(const TrackModel trackModel[], uint8_t len);

    uint8_t getTrackModelCache();

    /***
     * 获取寻迹标志位
     *
     *  @param result len = 6 | nullptr
     * @return
     */
    bool getTrackFlagBit(uint8_t* result);

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
    bool trafficLightRecognize_rigorous(K210Color* k210Color, uint16_t consecutiveEqualDegree = 3, uint16_t maxRetry = 24);

    /***
     * 测试连接用的
     * @return
     */
    bool ping();

    //void loop();

private:
    uint8_t trackModel{};
    bool receiveTrack;
    int8_t cameraSteeringGearAngle{};
};

struct TrackRowResult {
    uint8_t* flagBitArray;
    uint16_t flagBit;
    float offset;
    uint8_t bitCount;
    uint8_t centerBitCount;
    uint8_t edgeBitCount;
};

/***
 * 你的小车车
 * 一些和车有关的，杂七杂八的就扔在这里
 */
class Car : public DeviceBase {
public:
    explicit Car(uint8_t id);

    void carSleep(uint16_t time);

    /***
     * 清除车的码盘值
     */
    void clearCodeDisc();

    /***
     * 获取车的码盘值
     * @return
     */
    uint16_t getCodeDisc();

    /***
     * 获取寻迹灯
     */
    uint16_t getTrackLamp();

/***
 * 直行到下一个路口
 * @param carSpeed
 */
    void trackAdvanceToNextJunction();

    /***
     * 直行到下一个路口
     */
    void advanceToNextJunction();

    /***
     * 通过特殊地形
     * 然后调用advanceToNextJunction
     */
    void overspecificRelief();

    /***
     * 倒车寻迹到下一个路口
     */
    void recoilToNextJunction();

    /***
     * 前进一段距离
     * 无寻迹
     */
    void advance(uint16_t distance);

/***
 * 后退一定距离
 * 无寻迹
 */
    void recoil(uint16_t distance);

    void trackTurnLeft();

    void trackTurnRight();

    void turnLeft(uint8_t angle = 90);

    void turnRight(uint8_t angle = 90);

    /***
     * 微调车姿态
     */
    void trimCar();

    void trimCar(TrackRowResult* trackRowResult, float offset);

    /***
     * 矫正车身，与寻迹线平行
     */
    void rightCar();

    /***
     * 前进式调整
     * @param step
     */
    void advanceCorrection(uint16_t step, uint8_t maximumFrequency);

    /***
     * 来回式调整
     * @param step
     */
    void mobileCorrection(uint16_t step);

    bool acceptTrackFlag();


/***
 * led闪烁
 */
    void ledShine(int number, int wait);

    void onReceiveZigbeeMessage(uint8_t* buf) override;

    int16_t straightSpeed = 20;
    int16_t turningSpeed = 40;
    int16_t straightLineKpSpeed = 60;

    unsigned long trimOutTimeMs = 1000;
    unsigned long outTimeMs = 20000;


private:
    uint8_t flagBitArray[6] = {};
    TrackRowResult trackRowResultHigh{flagBitArray + 0};
    TrackRowResult trackRowResult{flagBitArray + 2};
    TrackRowResult trackRowResultLow{flagBitArray + 3};

    void rotationProcess();

    /***
     * 等待码盘
     * @param distance 等待码盘的变化量达到指定值
     */
    void waitCodeDisc(int16_t distance);

    void acceptTrackRowFlag(TrackRowResult* trackRowResult);
};

class MainCar : public DeviceBase {
public:
    explicit MainCar(uint8_t id);


    void onReceiveZigbeeMessage(uint8_t* buf) override;

};

class NetSynchronization {
public:

    /***
     * 全局变量
     */
    uint16_t globalVariable[256] = {};

    uint8_t globalVariableReturnCount = 0;

    /***
    * 自动同步一个全局数据
    * @param name [0,255]
    * @param value
    */
    bool synchronousGlobalVariable(uint8_t name, uint16_t value);

    /***
     * 同步连续的数据
     */
    bool synchronousGlobalVariable(uint8_t name, uint16_t* value, uint8_t len);

    bool synchronousGlobalVariable(uint8_t name, uint8_t* value, uint8_t len);

    uint16_t getGlobalVariable(uint8_t name);

    void getGlobalVariable(uint8_t name, uint16_t* buf, uint8_t len);

    void getGlobalVariable(uint8_t name, uint8_t* buf, uint8_t len);


#if FOR_THE_KING_HEN
    uint8_t licensePlateNumberHighReturnCount = 0;

    uint8_t licensePlateNumberLowReturnCount = 0;
#endif

    /***
     * 同步车牌号信息
     * 校验地址:173
     * 起始地址位:174
     * 数据长度:6
     * @param data len = 6
     * @return
     */
    bool synchronousLicensePlateNumber(uint8_t* data);

    /***
     * 获取车牌号信息
     * @param buf len = 6
     * @return 是否是有效信息
     */
    bool getLicensePlateNumber(uint8_t* buf);

    /***
     * 同步路径信息
     * 校验地址:180
     * 起始地址:181
     * 最大长度:32
     */
    bool synchronousPathInformation(uint16_t* buf, uint8_t len);

    bool getPathInformation(uint16_t* buf, uint8_t maxLen, uint8_t* len);

    /***
     * 获取主车位置
     * 地址:213
     * @return
     */
    uint16_t getMainCarPos();

    /***
     * 同步从车位置
     * @return
     */
    bool synchronousEntourageCarPos(uint16_t pos);

    /***
     * 获取从车位置
     * 地址:214
     * @return
     */
    uint16_t getEntourageCarPos();

};


/***
 * 你的沙盘，来着蒸汽宏伟之力的自动驾驶
 */
class SandTable {

public:
    /***
     * 调用前进仅能前进到下一个路口，x,y偶数位置必定组成路口，不会要求停在非路口
     * @param startPosition
     * @param startDirection
     * @param endPos
     * @param endDirection
     */
    void moveTo(struct Pos startPosition, enum Direction startDirection, struct Pos endPos, enum Direction* endDirection);

    void adjustDirection(struct Pos pos, enum Direction current, enum Direction target);

    void move(struct Pos a, struct Pos b);
};

extern DeviceBase* allDeviceBase[256];

extern MessageBus messageBus;

/***
 * 智能道闸标控制
 */
extern BarrierGate barrierGateA;

/***
 * 公交站
 */
extern BusStop busStopA;

/***
 *  智能显示(大屏)
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
 * 无线充电
 */
extern WirelessCharging wirelessChargingA;

/***
 * 信息显示
 */
extern InformationDisplay informationDisplayA;
extern InformationDisplay informationDisplayB;
extern InformationDisplay informationDisplayC;

//===============红外==============

/***
 * 报警台
 */
extern AlarmDesk alarmDeskA;

/***
 * 路灯
 */
extern StreetLamp streetLampA;

/***
 * 立体显示物
 */
extern StereoscopicDisplay stereoscopicDisplayA;
//==============other==============

extern UltrasonicDevice ultrasonicDevice;

/***
 * 摄像头
 */
extern K230 k230;

extern NetSynchronization netSynchronization;

/***
 * 你的小车车
 */
extern Car car;

extern MainCar mainCar;

extern SandTable sandTable;

//==============总览==============



#endif //ENTOURAGE_CLION_DEVICE_H


