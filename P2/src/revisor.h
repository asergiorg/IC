#ifndef REVISOR_H
#define REVISOR_H

#include <Arduino.h>

bool userIsTyping();
bool readKeyboard(String &line);
bool classifyToken(const String &token, int index, uint32_t &message);
void sendUsCommand(String command);
void processCommand(String command);
void readResponse(uint32_t message);
void printPendingResponses();

#endif