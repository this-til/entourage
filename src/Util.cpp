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
    uint32_t count = 0;
    for (uint32_t i = 0; i < len; ++i) {
        for (uint8_t ii = 0; ii < 8; ii++) {
            if (value[i] & (1 << ii)) {
                count++;
            }
        }
    }
    return count;
}

uint32_t countBits(const uint8_t* value, uint32_t len, uint32_t starting, uint32_t end) {
    uint32_t count = 0;
    for (uint32_t i = starting; i < end; ++i) {
        if (getBit(value, len, (int32_t) i, true)) {
            count++;
        }
    }
    return count;
}

uint32_t countBits(uint8_t uint8) {
    return countBits(&uint8, 1);
}


uint8_t getBit(const uint8_t* value, uint32_t len, int32_t bit, bool fromLeftToRight) {
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

    bool clearAway = false;
    for (uint32_t i = mLen / 2; i < mLen; ++i) {
        if (clearAway) {
            setBit(outValue, len, (int32_t) i, false, true);
            continue;
        }
        if (!getBit(value, len, (int32_t) i, true)) {
            clearAway = true;
        }
    }

    clearAway = false;
    for (int32_t i = (int32_t) mLen / 2; i >= 0; --i) {
        if (clearAway) {
            setBit(outValue, len, (int32_t) i, false, true);
            continue;
        }
        if (!getBit(value, len, (int32_t) i, true)) {
            clearAway = true;
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
    float _centerShift = centralPoint - (float) mLen / 2;
    *centerShift = _centerShift / ((float) mLen / 2 - 0.5f);
    return centralPoint;
}
