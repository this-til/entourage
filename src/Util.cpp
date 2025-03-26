//
// Created by til on 2025/3/1.
//

#include "Util.h"

const char hex_chars[] = "0123456789ABCDEF";

void logHex(uint8_t p) {
    Serial.print(hex_chars[(p >> 4) & 0x0F]);
    Serial.print(hex_chars[p & 0x0F]);
}

void logHex(uint16_t p) {
    Serial.print(hex_chars[(p >> 12) & 0x0F]);
    Serial.print(hex_chars[(p >> 8) & 0x0F]);
    Serial.print(hex_chars[(p >> 4) & 0x0F]);
    Serial.print(hex_chars[p & 0x0F]);
}

void logHex(const uint8_t* p, uint16_t len) {
    for (uint16_t i = 0; i < len; ++i) {
        logHex(p[i]);
    }
}

void logHex(const uint16_t* p, uint16_t len) {
    for (uint16_t i = 0; i < len; ++i) {
        logHex(p[i]);
    }
}

void logBool(bool b) {
    if (b) {
        Serial.print("true");
    } else {
        Serial.print("false");
    }
}

void logBin(uint8_t data) {
    for (int i = 7; i >= 0; --i) {
        if ((data >> i) & 0x01) {
            Serial.print("1");
        } else {
            Serial.print("0");
        }
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
}

void logObj(TimingMode timingMode) {
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
}

void logObj(TrafficLightModel trafficLightModel) {
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
}

void logObj(K210Color k210Color) {
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
}

bool equals(uint16_t* a, uint16_t* b, uint8_t len) {
    for (int i = 0; i < len; ++i) {
        if (a[i] != b[i]) {
            return false;
        }
    }
    return true;
}

TrafficLightModel k230ColorToTrafficLightModel(K210Color k210Color) {
    switch (k210Color) {
        case K_RED :
            return RED;
        case K_GREEN:
            return GREEN;
        case K_YELLOW:
            return YELLOW;
    }
    return YELLOW;
}

uint32_t countBits(const uint8_t* value, uint32_t len) {
    //return countBits(value, len, 0, len * 8);

    uint32_t add = 0;
    for (int i = 0; i < len; ++i) {
        add += bitCounts[value[i]];
    }
    return add;
}

uint32_t countBits(const uint8_t* value, uint32_t len, uint32_t starting, uint32_t end) {
    /*uint32_t count = 0;
    for (uint32_t i = starting; i < end; ++i) {
        if (getBit(value, len, (int32_t) i, true)) {
            count++;
        }
    }
    return count;*/

    if (starting >= end || end > len * 8) {
        return 0;
    }

    uint32_t start_byte = starting / 8;
    uint32_t end_byte = (end - 1) / 8;

    if (start_byte >= len || end_byte >= len) {
        return 0;
    }

    // Convert to bit positions within the byte (0 is the highest bit, 7 is the lowest)
    uint32_t s_bit = 7 - (starting % 8);
    uint32_t e_bit = 7 - ((end - 1) % 8);

    uint32_t count = 0;

    if (start_byte == end_byte) {
        // Handle single byte
        if (s_bit >= e_bit) {
            uint32_t mask_bits = s_bit - e_bit + 1;
            uint8_t mask = ((1 << mask_bits) - 1) << e_bit;
            count += bitCounts[value[start_byte] & mask];
        }
    } else {
        // Process start byte
        uint32_t mask_bits = s_bit + 1;
        uint8_t mask = (1 << mask_bits) - 1;
        count += bitCounts[value[start_byte] & mask];

        // Process full bytes between start and end
        for (uint32_t i = start_byte + 1; i < end_byte; ++i) {
            count += bitCounts[value[i]];
        }

        // Process end byte
        mask_bits = 7 - e_bit + 1;
        mask = ((1 << mask_bits) - 1) << e_bit;
        count += bitCounts[value[end_byte] & mask];
    }


    return count;
}

uint32_t countBits(uint8_t uint8) {
    return countBits(&uint8, 1);
}

Pos pack(uint16_t v) {
    Pos pos;
    pos.x = v >> 8;
    pos.y = v & 0xFF;
    return pos;
}

uint16_t unpack(Pos pos) {
    return pos.x << 8 | pos.y;
}

uint8_t getBit(const uint8_t* value, uint32_t len, int32_t bit, bool fromLeftToRight) {
/*    if (bit < 0 || bit >= len * 8) {
        return false;
    }

    if (fromLeftToRight) {
        return (value[bit / 8] >> (7 - (bit % 8))) & 0x01;
    } else {
        return value[len - (bit / 8) - 1] >> (bit % 8) & 0x01;
    }*/
    if (bit < 0 || bit >= len * 8) return 0;
    // 位运算替代除模运算，消除分支
    const uint32_t byteIdx = fromLeftToRight ? (bit >> 3) : (len - (bit >> 3) - 1);
    const uint8_t bitOffset = fromLeftToRight ? (7 - (bit & 7)) : (bit & 7);
    return (value[byteIdx] >> bitOffset) & 0x01;
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


float centralPoint(uint8_t* value, uint32_t len, float* centerShift, float* centerShiftOne) {
    /* uint32_t mLen = len * 8;
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
         if (centerShift != nullptr) {
             *centerShift = 0;
         }
         if (centerShiftOne != nullptr) {
             *centerShiftOne = 0;
         }
         return 0;
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
     if (centerShift != nullptr) {
         *centerShift = _centerShift;
     }
     if (centerShiftOne != nullptr) {
         *centerShiftOne = _centerShift / ((float) mLen / 2 - 0.5f);
     }
     return centralPoint;*/

    const uint32_t mLen = len * 8;
    int16_t startingPoint = -1;
    int16_t endPoint = -1;

    // Find the first set bit from left to right (startingPoint)
    for (uint32_t byte_idx = 0; byte_idx < len; ++byte_idx) {
        const uint8_t byte_val = value[byte_idx];
        if (byte_val != 0) {
            const int highest_bit = highestBitTable[byte_val];
            startingPoint = byte_idx * 8 + (7 - highest_bit);
            break;
        }
    }

    if (startingPoint < 0) { // All bits are zero
        if (centerShift) {
            *centerShift = 0.0f;
        }
        if (centerShiftOne) {
            *centerShiftOne = 0.0f;
        }
        return -1;
    }

    // Find the first set bit from right to left (endPoint)
    for (int32_t byte_idx = len - 1; byte_idx >= 0; --byte_idx) {
        const uint8_t byte_val = value[byte_idx];
        if (byte_val != 0) {
            const int lowest_bit = lowestBitTable[byte_val];
            endPoint = byte_idx * 8 + (7 - lowest_bit);
            break;
        }
    }

    const float centralPoint = startingPoint + (endPoint - startingPoint) * 0.5f + 0.5f;
    const float _centerShift = centralPoint - static_cast<float>(mLen) * 0.5f;

    if (centerShift) {
        *centerShift = _centerShift;
    }
    if (centerShiftOne) {
        const float scale = static_cast<float>(mLen) * 0.5f - 0.5f;
        *centerShiftOne = _centerShift / scale;
    }

    return centralPoint;
}

uint16_t assembly(const char* str) {
    return ((uint8_t) str[0] << 8) | (uint8_t) str[1];
}

void assembly(const char* str, uint16_t* outArray, uint8_t maxArrayLen) {
    uint8_t count = 0;
    while (count < maxArrayLen && *str && *(str + 1)) {
        outArray[count] = (uint16_t) ((*str) << 8 | *(str + 1));
        str += 2;
        count++;
    }
}

void analyze(uint16_t* array, uint8_t maxArrayLen, char* outStr, uint8_t maxStrLen) {
    uint8_t outIndex = 0; // 记录输出字符串的当前位置
    for (uint8_t i = 0; i < maxArrayLen; ++i) {
        // 检查剩余空间是否足够写入两个字符 + 终止符
        if (outIndex + 2 >= maxStrLen) break;

        // 提取高8位和低8位，转换为字符
        char high = static_cast<char>((array[i] >> 8) & 0xFF);
        char low = static_cast<char>(array[i] & 0xFF);

        // 写入输出字符串
        outStr[outIndex++] = high;
        outStr[outIndex++] = low;
    }
    // 确保字符串以'\0'结尾
    outStr[outIndex] = '\0';
}

void excludeSpecialCharacter(const uint8_t* source, uint16_t sourceLen, uint8_t* out, uint16_t outMaxLen, uint16_t* outLen) {
    uint8_t outIndex = 0;
    for (uint8_t i = 0; i < sourceLen; ++i) {
        uint8_t c = source[i];
        if ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')) {
            out[outIndex] = c;
            outIndex++;
            if (outIndex >= outMaxLen) {
                break;
            }
        }
    }
    if (outLen != nullptr) {
        *outLen = outIndex;
    }
}

void excludeCharacter(const uint8_t* source, uint16_t sourceLen, uint8_t* out, uint16_t outMaxLen, uint16_t* outLen, uint8_t* matchingItem) {

    uint8_t outIndex = 0;
    for (uint8_t i = 0; i < sourceLen; ++i) {
        uint8_t c = source[i];
        if (matchingItem[c]) {
            continue;
        }
        out[outIndex] = c;
        outIndex++;
        if (outIndex >= outMaxLen) {
            break;
        }
    }

    if (outLen != nullptr) {
        *outLen = outIndex;
    }

}

void extractCharacter(const uint8_t* str, uint16_t strLen, uint8_t* outStr, uint16_t maxLen, uint16_t* outLen, uint8_t up, uint8_t down, bool inRadius) {
    uint16_t l = 0;
    bool _inRadius = false;
    for (int i = 0; i < strLen; ++i) {
        if (str[i] == up && !_inRadius) {
            _inRadius = true;
            continue;
        }

        if (str[i] == down && _inRadius) {
            _inRadius = false;
            if (inRadius) {
                break;
            }
            continue;
        }

        if (inRadius == _inRadius) {
            outStr[l++] = str[i];
            if (l >= maxLen) {
                break;
            }
        }
    }

    if (outLen != nullptr) {
        *outLen = l;
    }
}

bool inclusiveCharacter(const uint8_t* str, uint16_t strLen, uint8_t* matchingEntry, uint16_t matchingEntryLen, uint16_t* preferredMatch) {
    // 边界条件检查：匹配项为空或源字符串长度不足
    if (matchingEntryLen == 0 || strLen < matchingEntryLen) {
        return false;
    }

    // 计算最大需要检查的起始位置
    const uint16_t maxStartPos = strLen - matchingEntryLen;

    // 遍历所有可能的起始位置
    for (uint16_t i = 0; i <= maxStartPos; i++) {
        // 检查当前位置是否匹配
        bool isMatch = true;
        for (uint16_t j = 0; j < matchingEntryLen; j++) {
            if (str[i + j] != matchingEntry[j]) {
                isMatch = false;
                break;
            }
        }

        // 找到匹配时设置偏移量并返回
        if (isMatch) {
            if (preferredMatch != nullptr) {
                *preferredMatch = i;
            }
            return true;
        }
    }

    // 未找到匹配项
    return false;
}


void spotTheDifference(const uint8_t* strA, const uint8_t* strB, uint16_t strLen, uint8_t* out, uint16_t maxOutLen, uint16_t* countOfDifferentTerms) {
    if (maxOutLen % 2 == 1) {
        maxOutLen--;
    }
    int maxItem = maxOutLen / 2;
    int item = 0;
    for (int i = 0; i < strLen; ++i) {
        if (strA[i] != strB[i]) {

            out[item] = strA[i];
            out[maxOutLen - item - 1] = strB[i];

            item++;
            if (item >= maxItem) {
                break;
            }

        }
    }
    if (countOfDifferentTerms != nullptr) {
        *countOfDifferentTerms = item;
    }
}

void asciiToHex(const uint8_t* str, uint16_t strLen, uint8_t* out, uint16_t maxOutLen, uint16_t* outLen) {
    uint16_t len = 0;
    for (uint16_t i = 0; i + 1 < strLen && len < maxOutLen; i += 2) {
        // 处理高四位字符
        uint8_t high = str[i];
        uint8_t highVal = 0;
        if (high >= '0' && high <= '9') {
            highVal = high - '0';
        } else if (high >= 'A' && high <= 'F') {
            highVal = 10 + (high - 'A');
        } else if (high >= 'a' && high <= 'f') {
            highVal = 10 + (high - 'a');
        }

        // 处理低四位字符
        uint8_t low = str[i + 1];
        uint8_t lowVal = 0;
        if (low >= '0' && low <= '9') {
            lowVal = low - '0';
        } else if (low >= 'A' && low <= 'F') {
            lowVal = 10 + (low - 'A');
        } else if (low >= 'a' && low <= 'f') {
            lowVal = 10 + (low - 'a');
        }

        // 合并高低四位并存入输出缓冲区
        out[len++] = (highVal << 4) | lowVal;
    }
    *outLen = len; // 设置实际转换后的数据长度
}

#define MAX_STACK_SIZE 8

typedef struct {
    int16_t data[MAX_STACK_SIZE];
    int top;
} OperandStack;

typedef struct {
    uint8_t data[MAX_STACK_SIZE];
    int top;
} OperatorStack;

/* 辅助函数 */
static void init_operand(OperandStack* s) { s->top = -1; }

static void push_operand(OperandStack* s, int16_t v) {
    if (s->top < MAX_STACK_SIZE - 1) s->data[++s->top] = v;
}

static int16_t pop_operand(OperandStack* s) {
    return (s->top >= 0) ? s->data[s->top--] : 0;
}

static void init_operator(OperatorStack* s) { s->top = -1; }

static void push_operator(OperatorStack* s, uint8_t op) {
    if (s->top < MAX_STACK_SIZE - 1) s->data[++s->top] = op;
}

static uint8_t pop_operator(OperatorStack* s) {
    return (s->top >= 0) ? s->data[s->top--] : 0;
}

static uint8_t peek_operator(OperatorStack* s) {
    return (s->top >= 0) ? s->data[s->top] : 0;
}

/* 运算符优先级 */
static int priority(uint8_t op) {
    switch (op) {
        case '^':
            return 4;
        case '*':
        case '/':
            return 3;
        case '+':
        case '-':
            return 2;
        case '(':
            return 1;
        default:
            return 0;
    }
}

/* 幂计算 */
static int16_t power(int16_t a, int16_t b) {
    int16_t res = 1;
    for (int i = 0; i < b; i++) res *= a;
    return res;
}

/* 执行运算 */
static void apply_op(uint8_t op, OperandStack* os) {
    int16_t b = pop_operand(os), a = pop_operand(os), res = 0;
    switch (op) {
        case '+':
            res = a + b;
            break;
        case '-':
            res = a - b;
            break;
        case '*':
            res = a * b;
            break;
        case '/':
            if (b != 0) res = a / b;
            break;
        case '^':
            res = power(a, b);
            break;
    }
    push_operand(os, res);
}

int16_t evaluateTheExpression(const uint8_t* expr, uint16_t var[]) {
    OperandStack op_stack;
    OperatorStack oper_stack;
    init_operand(&op_stack);
    init_operator(&oper_stack);

    for (int i = 0; expr[i];) {
        uint8_t c = expr[i];
        if (isalpha(c)) {  // 处理变量
            push_operand(&op_stack, (int16_t) var[c]);
            i++;
        } else if (isdigit(c)) {  // 处理数字
            uint16_t num = 0;
            while (isdigit(expr[i]))
                num = num * 10 + (expr[i++] - '0');
            push_operand(&op_stack, (int16_t) num);
        } else if (c == '(') {  // 左括号
            push_operator(&oper_stack, c);
            i++;
        } else if (c == ')') {  // 右括号
            while (peek_operator(&oper_stack) != '(')
                apply_op(pop_operator(&oper_stack), &op_stack);
            pop_operator(&oper_stack);  // 弹出'('
            i++;
        } else {  // 运算符
            while (!(oper_stack.top == -1 || peek_operator(&oper_stack) == '(' ||
                     (priority(c) > priority(peek_operator(&oper_stack))) ||
                     (priority(c) == priority(peek_operator(&oper_stack)) && c == '^'))) {
                apply_op(pop_operator(&oper_stack), &op_stack);
            }
            push_operator(&oper_stack, c);
            i++;
        }
    }

    // 处理剩余运算符
    while (oper_stack.top != -1) {
        apply_op(pop_operator(&oper_stack), &op_stack);
    }

    return pop_operand(&op_stack);
}
