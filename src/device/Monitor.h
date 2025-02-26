//
// Created by til on 2025/2/26.
//

#ifndef ENTOURAGE_CLION_MONITOR_H
#define ENTOURAGE_CLION_MONITOR_H

#include "def.h"

enum TimingMode {
    TIMING_OFF = 0x00,
    TIMING_ON = 0x01,
    CLOCK_RESET = 0x02,
};

/***
 *  智能显示标志物控制
 */

class Monitor {
public:
    Monitor(int id);

    /***
     * 设置数码管显示指定数据
     * @param pos 第几个数码管 1:第一排 2:第二排
     * @param value[] 值 0~9 长度6
     */
    void setDisplayData(uint8_t pos, uint8_t value[]);

    /***
     * 设置第一排数码管显示计时模式
     */
    void setTimingMode(TimingMode timingMode);

    /***
     * 智能显示标志物第二排数码管显示距离模式，第二位和第三位副指令中“X”为要显示的距离值，单位为毫米，显示格式为十进制。
     * @param distance_mm 距离单位毫米
     */
    void setDistance(uint16_t distance_mm);

private:
    uint8_t id;
};


extern Monitor monitorA;

#endif //ENTOURAGE_CLION_MONITOR_H
