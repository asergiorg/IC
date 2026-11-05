#ifndef TOKEN_CLASSIFYER_H
#define TOKEN_CLASSIFYER_H

#include <Arduino.h>

uint32_t getFirstParameterValue(const String &token);
uint32_t getSecondParameterValue(const String &token);
String getFirstParameter(uint8_t code);
String getSecondParameter(uint8_t code);

#endif