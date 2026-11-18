#include "can_communications.h"
#include "response_queue.h"
#include <CAN.h>

static void onCanReceive(int packetSize) {
    CanPacket packet;
    packet.length = 0;
    while (CAN.available() && packet.length < 8) {
        packet.data[packet.length++] = (uint8_t)CAN.read();
    }
    if (packet.length > 0) pushPacketFromISR(packet);
}

void canBegin() {
    if (!CAN.begin(500E3)) {
        Serial.println("Could not initiate CAN");
        while (1);
    }
    CAN.onReceive(onCanReceive);
}

void send(uint32_t data) {
    CAN.beginPacket(0x12);
    for (int i = 3; i >= 0; i--) CAN.write((data >> (i * 8)) & 0xFF);
    CAN.endPacket();
}