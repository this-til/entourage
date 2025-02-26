//
// Created by til on 2025/2/26.
//

#ifndef ENTOURAGE_CLION_BUSSTOP_H
#define ENTOURAGE_CLION_BUSSTOP_H

#include "def.h"

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
    OVERCAST_SKY = 0x05,
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

class BusStop {
public:
    BusStop(uint8_t id);

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
    void getRtcDate(uint8_t* yy, uint8_t* MM, uint8_t* dd);

    /***
     * 查询RTC当前时间
     */
    void getRtcTime(uint8_t* HH, uint8_t* mm, uint8_t* ss);

    /***
     * 设置天气数据与温度数据
     */
    void setWeatherTemperature(Weather weather, uint8_t temperature);

    /***
     * 请求回传天气数据与温度数据
     */
    void getWeatherTemperature(Weather* weather, uint8_t* temperature);

    /***
     * 开始合成语言
     */
    void startSynthesizingLanguage(uint8_t* data, uint16_t len, TextEncodingFormat textEncodingFormat);

    /***
       * 暂停合成语言
       */
    void pauseSynthesizingLanguage();

    /***
       * 恢复合成语言
       */
    void recoverSynthesizingLanguage();

private:
    uint8_t id;
};

extern BusStop busStopA;

#endif //ENTOURAGE_CLION_BUSSTOP_H
