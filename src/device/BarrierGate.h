//
// Created by til on 2025/2/26.
//

#ifndef ENTOURAGE_CLION_BARRIERGATE_H
#define ENTOURAGE_CLION_BARRIERGATE_H

#include "def.h"

/***
 * 智能道闸标控制
 */
class BarrierGate {
public:
    BarrierGate(uint8_t id);

    /***
     * 设置门闸
     * @param open id true 开启
     */
    void setGateControl(bool open);

    /***
     * 获取门闸
     * @param awaitTimeMs 等待时间，由于 "智能道闸标志物处于关闭状态时请求回传状态，不会回传任何指令。" 设定等待时间 ??? 离谱的需求
     * @return id true 开启
     */
    bool getGateControl(int awaitTimeMs = 100);

private:
    uint8_t id;
};

extern BarrierGate barrierGateA;

#endif //ENTOURAGE_CLION_BARRIERGATE_H
