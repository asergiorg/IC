#include "token_classifyer.h"
#include "string_manipulation.h"

uint32_t getFirstParameterValue(const String &token) {
    if (token == "one-shot") return 8UL << 12;
    else if (token == "on") return 1UL << 12;
    else if (token == "off") return 14UL << 12;
    else if (token == "unit") return 7UL << 12;
    else if (token == "delay") return 4UL << 12;
    else if (token == "status") return 13UL << 12;
}

uint32_t getSecondParameterValue(const String &token) {
    if (token == "inc") return 1UL << 7;
    else if (token == "cm") return 2UL << 7;
    else if (token == "ms") return 3UL << 7;
  
    // Period/delay in ms
    else if (isNumber(token)) return (uint32_t)token.toInt();
}

String getFirstParameter(uint8_t code) {
    String parameter; 
    switch (code){
    case 1:
        parameter = "on";
        break;
    case 4:
        parameter = "delay";
        break;
    case 7:
        parameter = "unit";
        break;
    case 8:
        parameter = "one-shot";
        break;
    case 13:
        parameter = "status";
        break;
    case 14:
        parameter = "off";
        break;
    default:
        parameter = "";
        break;
    }
    return parameter;
}

String getSecondParameter(uint8_t code) {
    String parameter; 
    switch (code){
    case 1:
        parameter = "inc";
        break;
    case 2:
        parameter = "cm";
        break;
    case 3:
        parameter = "ms";
        break;
    default:
        parameter = "";
        break;
    }
    return parameter;
}