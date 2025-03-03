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


uint32_t countBits(const uint8_t* value, uint32_t len) {
    uint16_t count = 0;
    for (int i = 0; i < len; ++i) {
        for (uint8_t ii = 0; ii < 8; ii++) {
            if (value[i] & (1 << ii)) {
                count++;
            }
        }
    }
    return count;
}

uint32_t countBits(uint8_t uint8) {
    return countBits(&uint8, 1);
}


uint8_t getBit(uint8_t* value, uint32_t len, int32_t bit, bool fromLeftToRight) {
    if (bit < 0 || bit >= len * 8) {
        return false;
    }

    if (fromLeftToRight) {
        return (value[bit / 8] >> (7 - (bit % 8))) & 0x01;
    } else {
        return value[len - (bit / 8) - 1] >> (bit % 8) & 0x01;
    }
}

void setBit(uint8_t* value, uint32_t len, int32_t bit, bool set, bool fromLeftToRight) {
    if (bit < 0 || bit >= len * 8) {
        return;
    }
    if (fromLeftToRight) {
        if (set) {
            value[bit / 8] |= 0x80 >> (bit % 8);
        } else {
            value[bit / 8] &= ~(0x80 >> (bit % 8));
        }
    } else {
        if (set) {
            value[len - (bit / 8) - 1] |= (0x01 << (bit % 8));
        } else {
            value[len - (bit / 8) - 1] &= ~(0x01 << (bit % 8));
        }
    }
}

void lonelinessExclusion(uint8_t* value, uint32_t len, uint8_t* outValue) {
    uint32_t mLen = len * 8;

    for (uint32_t i = 0; i < len; ++i) {
        outValue[i] = value[i];
    }


    for (int32_t i = 0; i < mLen; ++i) {
        int32_t beforeIn = i - 1;
        int32_t afterIn = i + 1;

        bool before = getBit(value, len, beforeIn, true);
        //bool current = (value[i / 8] >> (i % 8)) & 0x01;
        bool after = getBit(value, len, afterIn, true);

        if (!before && !after) {
            setBit(outValue, len, i, false, true);
        }

    }
}

float centralPoint(uint8_t* value, uint32_t len, float* centerShift) {
    uint32_t mLen = len * 8;
    float startingPoint = -1;
    float endPoint = -1;

    for (int32_t i = 0; i < mLen; ++i) {
        bool bit = getBit(value, len, i, true);
        if (bit) {
            startingPoint = (float) i;
            break;
        }
    }

    if (startingPoint == -1) {
        *centerShift = 0;
        return -1;
    }

    for (int32_t i = 0; i < mLen; ++i) {
        bool bit = getBit(value, len, i, false);
        if (bit) {
            endPoint = (float) (mLen - i - 1);
            break;
        }
    }

    float centralPoint = startingPoint + ((endPoint - startingPoint) / 2);
    centralPoint += 0.5;
    *centerShift = centralPoint - (float) mLen / 2;
    return centralPoint;
}
