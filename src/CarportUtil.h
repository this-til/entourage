//
// Created by til on 2025/2/26.
//

#ifndef ENTOURAGE_CLION_CARPORTUTIL_H
#define ENTOURAGE_CLION_CARPORTUTIL_H

#include "Command.h"
#include "ExtSRAMInterface.h"


#include <Arduino.h>

/***
 * 将车库移动到指定层
 * @param level 指定层
 */
void carport_moveToLevel(uint8_t level);


/***
 * 获取车库层数
 * @return
 */
uint8_t carport_getLevel();

#endif //ENTOURAGE_CLION_CARPORTUTIL_H
