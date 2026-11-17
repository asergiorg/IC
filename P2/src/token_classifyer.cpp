#include "token_classifyer.h"
#include "string_manipulation.h"

uint32_t getOperationValue(const String &token) {
    if (token == "help") return 0UL << 24;
    if (token == "us")   return 1UL << 24;
    return 0;
}

uint32_t getSensorValue(uint8_t sensor) {
    return (uint32_t)sensor << 16;
}

uint32_t getFirstParameterValue(const String &token) {
    if (token == "one-shot") return 8UL  << 12;
    if (token == "on")       return 1UL  << 12;
    if (token == "off")      return 14UL << 12;
    if (token == "unit")     return 7UL  << 12;
    if (token == "delay")    return 4UL  << 12;
    if (token == "status")   return 13UL << 12;
    return 0;
}

uint32_t getSecondParameterValue(const String &token) {
    if (token == "inc") return 1UL << 8;
    if (token == "cm")  return 2UL << 8;
    if (token == "ms")  return 3UL << 8;
    return 0;
}

uint32_t getValueField(uint8_t milliseconds) {
    return milliseconds;
}

String getFirstParameter(uint8_t code) {
    switch (code) {
        case 1:  return "on";
        case 4:  return "delay";
        case 7:  return "unit";
        case 8:  return "one-shot";
        case 13: return "status";
        case 14: return "off";
        default: return "";
    }
}

String getSecondParameter(uint8_t code) {
    switch (code) {
        case 1:  return "inc";
        case 2:  return "cm";
        case 3:  return "ms";
        default: return "";
    }
}