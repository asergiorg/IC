#include "response_queue.h"

#define QUEUE_SIZE 64

static CanPacket queueData[QUEUE_SIZE];
static volatile uint8_t  qHead = 0;
static volatile uint8_t  qTail = 0;
static volatile uint16_t droppedMessages = 0;

void pushPacketFromISR(const CanPacket &packet) {
    uint8_t next = (qHead + 1) & (QUEUE_SIZE - 1);
    if (next == qTail) {
        droppedMessages++;
        return;
    }
    queueData[qHead] = packet;
    __sync_synchronize();
    qHead = next;
}

bool popPacket(CanPacket &packet) {
    if (qTail == qHead) return false;
    packet = queueData[qTail];
    __sync_synchronize();
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