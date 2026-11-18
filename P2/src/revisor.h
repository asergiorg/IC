#ifndef REVISOR_H
#define REVISOR_H

#include <Arduino.h>
#include "response_queue.h"

#define RESPONSE_TIMEOUT_MS 500

bool readKeyboard(String &line);
bool userIsTyping();
bool processCommand(String command);
void printPendingResponses();
void requestPrompt(uint32_t delayMs);
void updatePrompt();

#endif