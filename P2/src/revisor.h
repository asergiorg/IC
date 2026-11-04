#ifndef REVISOR_H
#define REVISOR_H

#include <Arduino.h>

void prepareShell();
String readKeyboard() ;
void proccessCommand(String command);
void sendUsCommand(String command);
uint32_t classifyToken(const String &token, int index, uint32_t message);

#endif