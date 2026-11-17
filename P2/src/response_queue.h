#ifndef RESPONSE_QUEUE_H
#define RESPONSE_QUEUE_H
#include <Arduino.h>

void pushResponseFromISR(uint32_t msg);
bool popResponse(uint32_t &msg);
uint16_t takeDroppedMessages();

#endif