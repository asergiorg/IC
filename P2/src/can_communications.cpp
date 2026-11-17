#include "can_communications.h"
#include "response_queue.h"
#include <CAN.h>

static void onCanReceive(int packetSize) {
    if (packetSize < 4) return;
    uint32_t msg = 0;
    for (int i = 0; i < 4; i++) {
        msg = (msg << 8) | (uint8_t)CAN.read();
    }
    pushResponseFromISR(msg);
}

void canBegin() {
    if (!CAN.begin(500E3)) {
        Serial.println("Error iniciando CAN");
        while (1);
    }
    CAN.onReceive(onCanReceive);
}

void send(uint32_t data) {
    CAN.beginPacket(0x12);
    for (int i = 3; i >= 0; i--) CAN.write((data >> (i * 8)) & 0xFF);
    CAN.endPacket();
}