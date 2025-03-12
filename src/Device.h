//
// Created by til on 2025/2/27.
//

#ifndef ENTOURAGE_CLION_DEVICE_H
#define ENTOURAGE_CLION_DEVICE_H

#include "def.h"
#include "Enum.h"

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
    bool readHeadIntegrity;
    unsigned long readOutTime_ms;
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
class Carport : DeviceBase {

public:

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

    void setLightSourceIntensity(LightSourceIntensity lightSourceIntensity);
};

/***
 * 信息显示物(广告牌)
 */
class AdvertisingBoard : DeviceBase {
    explicit AdvertisingBoard(uint8_t id);
};

/***
 * 无线充电
 */
class WirelessCharging : DeviceBase {
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
class InformationDisplay : DeviceBase {
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

class ETC : DeviceBase {

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


class StereoscopicDisplay : DeviceBase {
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
    bool trafficLightRecognize_rigorous(K210Color* k210Color, uint16_t consecutiveEqualDegree = 3, uint16_t maxRetry = 48);

    /***
     * 测试连接用的
     * @return
     */
    bool ping();

    //void loop();

private:
    uint8_t trackModel;
    bool receiveTrack;
    int8_t cameraSteeringGearAngle;
};

struct TrackRowResult {
    uint8_t* flagBitArray;
    uint16_t flagBit;
    float offset;
    uint8_t bitCount;
    uint8_t centerBitCount;
    uint8_t edgeBitCount;
};

struct TrackResult {
    uint8_t flagBitArray[6] = {};
    TrackRowResult trackRowResultHigh{};
    TrackRowResult trackRowResult{};
    TrackRowResult trackRowResultLow{};
};


class MainCar : DeviceBase {
public:
    explicit MainCar(uint8_t id);

    bool awaitReturn(uint8_t buf[], uint8_t serviceId);
};

/***
 * 你的小车车
 * 一些和车有关的，杂七杂八的就扔在这里
 */
class Car {
public:
    Car();

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

    void trackTurnLeft(bool trimCar = true);

    void trackTurnRight(bool trimCar = true);

    void turnLeft(bool trimCar = true);

    void turnRight(bool trimCar = true);

    /***
     * 微调车姿态
     */
    void trimCar();

    /***
     * 矫正车身，与寻迹线平行
     */
    void rightCar();

    void mobileCorrection(uint16_t step);

    bool acceptTrackFlag();


/***
 * led闪烁
 */
    void ledShine(int number, int wait);

    int16_t straightLineSpeed;
    int16_t turnLeftSpeed;
    int16_t turnRightSpeed;
    int16_t straightLineKpSpeed;
    int16_t trimKpSpeed;

    unsigned long trimOutTime_ms;
    unsigned long outTime_ms;


private:
    TrackResult trackResult{};

    void rotationProcess();

    /***
     * 等待码盘
     * @param 等待码盘的变化量达到指定值(不检测正反)
     */
    void waitCodeDisc(int16_t distance);

    void acceptTrackRowFlag(TrackRowResult* trackRowResult);
};


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
/***
 * 你的小车车
 */
extern Car car;

extern MainCar mainCar;

#endif //ENTOURAGE_CLION_DEVICE_H
