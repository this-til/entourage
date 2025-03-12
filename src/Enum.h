//
// Created by til on 2025/3/11.
//

#ifndef ENTOURAGE_CLION_ENUM_H
#define ENTOURAGE_CLION_ENUM_H

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

enum PageTurningMode {
    UP = 0x01,
    UNDER = 0x02,
    AUTOMATIC_PAGE_TURNING = 0x03
};

enum InformationDisplayTimingMode {
    OFF = 0x01,
    NO = 0x02,
    ZERO = 0x03,
};

/***
 * 进制格式
 */
enum BaseFormat {
    //16
    F_HEX = 0x40,
    //10
    F_DEC = 0x50
};

enum Graph {
    /***
     * 矩形,
     */
    RECTANGLE = 0x01,
    /***
     * 圆形,
     */
    ROUNDNESS = 0x02,
    /***
     * 三角形,
     */
    TRIANGLE = 0x03,
    /***
     * 菱形,
     */
    RHOMBOID = 0x04,
    /***
     * 五角星
     */
    FIVE_POINTED_STAR = 0x05
};

enum StereoscopicDisplayColor {
    /***
     * 红色
     */
    SD_RED = 0x01,
    /***
     * 绿色
     */
    SD_GREEN = 0x02,
    /***
     * 蓝色
     */
    SD_BLUE = 0x03,
    /***
     * 黄色
     */
    SD_YELLOW = 0x04,
    /***
     * 品色
     */
    SD_CHROMATISM = 0x05,
    /***
     * 青色
     */
    SD_CYAN = 0x06,
    /***
     * 黑色
     */
    SD_BLACK = 0x07,
    /***
     * 白色
     */
    SD_WHITE = 0x08,
};

enum TrafficSign {
    /***
     * 直行
     */
    THEN_RUN = 0x01,
    /***
     * 左转
     */
    THEN_LEFT = 0x02,
    /***
     * 右转
     */
    THEN_RIGHT = 0x03,
    /***
     * 掉头
     */
    TURN_ROUND = 0x04,
    /***
     * 禁止直行
     */
    NO_STRAIGHT = 0x05,
    /***
     * 禁止通行
     */
    NO_ACCESS = 0x06,
};

enum TrafficWarningSigns {
    /***
     * 前方学校_减速慢行
     */
    SCHOOL_AHEAD___SLOW_DOWN = 0x01,
    /***
     * 前方施工_禁止通行
     */
    CONSTRUCTION_AHEAD___NO_ACCESS = 0x02,
    /***
     * 塌方路段_注意安全
     */
    LANDSLIDES___BE_SAFE = 0x03,
    /***
     * 追尾危险_保持车距
     */
    REAR_END_COLLISION_HAZARD___KEEP_DISTANCE = 0x04,
    /***
     * 严禁_酒后驾车
     */
    NO___DRINK_AND_DRIVE = 0x05,
    /***
     * 严禁_乱扔垃圾
     */
    STRICTLY_PROHIBITED___LITTERING = 0x06,
};

enum LightSourceIntensity {
    ONR = 0x0C,
    TWO = 0x18,
    THERE = 0x5E,
};

enum TrackModel {
    TRACK_LOW = 1 << 0,
    TRACK_MIDDLE = 1 << 1,
    TRACK_HIGH = 1 << 2
};

#endif //ENTOURAGE_CLION_ENUM_H
