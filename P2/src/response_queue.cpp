#include "response_queue.h"

#define QUEUE_SIZE 32

static volatile uint64_t queueData[QUEUE_SIZE];
static volatile uint8_t  qHead = 0;
static volatile uint8_t  qTail = 0;
static volatile uint16_t droppedMessages = 0;

void pushResponseFromISR(uint32_t msg) {
  uint8_t next = (qHead + 1) & (QUEUE_SIZE - 1);
  if (next == qTail) {
    droppedMessages++;
    return;
  }
  queueData[qHead] = msg;
  qHead = next;
}

bool popResponse(uint32_t &msg) {
  if (qTail == qHead) return false;
  msg = queueData[qTail];
  qTail = (qTail + 1) & (QUEUE_SIZE - 1);
  return true;
}

uint16_t takeDroppedMessages() {
  noInterrupts();
  uint16_t dropped = droppedMessages;
  droppedMessages = 0;
  interrupts();
  return dropped;
}