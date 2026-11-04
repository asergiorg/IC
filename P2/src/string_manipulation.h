#ifndef STRING_MANIPULATION_H
#define STRING_MANIPULATION_H

#include <Arduino.h>

int separate(String command, String separatedCommand[], int max);
bool isNumber(const String &token);

#endif