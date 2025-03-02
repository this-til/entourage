//
// Created by til on 2025/3/1.
//

#include "Util.h"

const char hex_chars[] = "0123456789ABCDEF";

void logHex(uint8_t p) {
    Serial.print(hex_chars[(p >> 4) & 0x0F]);
    Serial.print(hex_chars[p & 0x0F]);
}

void logHex(const uint8_t* p, uint16_t len) {
    for (uint16_t i = 0; i < len; ++i) {
        logHex(p[i]);
    }
}

void logObj(Weather weather) {
    switch (weather) {
        case GALE :
            Serial.print("GALE");
            return;
        case CLOUDY :
            Serial.print("CLOUDY");
            return;
        case SUNNY :
            Serial.print("SUNNY");
            return;
        case LIGHT_SNOW :
            Serial.print("LIGHT_SNOW");
            return;
        case SPIT :
            Serial.print("SPIT");
            return;
        case OVERCAST_SKY :
            Serial.print("OVERCAST_SKY");
            return;
    }
    Serial.print("NULL");
}

void logObj(TextEncodingFormat textEncodingFormat) {
#if DE_BUG
    switch (textEncodingFormat) {
        case GB2312:
            Serial.print("GB2312");
            return;
        case GBK:
            Serial.print("GBK");
            return;
        case BIG5:
            Serial.print("BIG5");
            return;
        case Unicode:
            Serial.print("Unicode");
            return;
    }
    Serial.print("NULL");
#endif
}

void logObj(TimingMode timingMode) {
#if DE_BUG
    switch (timingMode) {
        case TIMING_OFF :
            Serial.print("TIMING_OFF");
            return;
        case TIMING_ON :
            Serial.print("TIMING_ON");
            return;
        case CLOCK_RESET :
            Serial.print("CLOCK_RESET");
            return;
    }
    Serial.print("NULL");
#endif
}

void logObj(TrafficLightModel trafficLightModel) {
#if DE_BUG
    switch (trafficLightModel) {
        case RED:
            Serial.print("RED");
            return;
        case GREEN:
            Serial.print("GREEN");
            return;
        case YELLOW:
            Serial.print("YELLOW");
            return;
    }
    Serial.print("NULL");
#endif
}

void logObj(K210Color k210Color) {
#if DE_BUG
    switch (k210Color) {
        case K_NONE:
            Serial.print("NONE");
            return;
        case K_WHITE:
            Serial.print("WHITE");
            return;
        case K_BLACK:
            Serial.print("BLACK");
            return;
        case K_RED:
            Serial.print("RED");
            return;
        case K_GREEN:
            Serial.print("GREEN");
            return;
        case K_YELLOW:
            Serial.print("YELLOW");
            return;
    }
    Serial.print("NULL");
#endif
}

