//
// Created by til on 2025/2/26.
//

#ifndef ENTOURAGE_CLION_CARPORT_H
#define ENTOURAGE_CLION_CARPORT_H

#include "Command.h"
#include "ExtSRAMInterface.h"


#include "def.h"

/***
 * 智能立体车
 */
class Carport {

public:

    explicit Carport(uint8_t id);

    /***
    * 将车库移动到指定层
    * @param level 指定层
    */
    void moveToLevel(uint8_t level);


    /***
    * 获取车库层数
    * @return if 0 获取失败
    */
    uint8_t getLevel();

    /***
    * 获取车库前后侧红外状态
    * @param ventral out 前侧
    * @param rearSide out 后侧
    */
    void getInfraredState(bool* ventral, bool* rearSide);


private:

    uint8_t id;

};

extern Carport carportA;
extern Carport carportB;

#endif //ENTOURAGE_CLION_CARPORT_H
