#ifndef RESPONSE_QUEUE_H
#define RESPONSE_QUEUE_H
#include <Arduino.h>

struct CanPacket {
  uint8_t length;
  uint8_t data[8];
};

void pushPacketFromISR(const CanPacket &packet);
bool popPacket(CanPacket &packet);
uint16_t takeDroppedMessages();

#endif