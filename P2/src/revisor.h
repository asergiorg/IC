#ifndef REVISOR_H
#define REVISOR_H

#include <Arduino.h>

String readKeyboard();
uint32_t classifyToken(const String &token, int index, uint32_t message);
void sendUsCommand(String command);
void proccessCommand(String command);
void readResponse(uint32_t message);

#endif