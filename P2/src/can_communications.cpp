#include "can_communications.h"
#include <CAN.h>

void send(uint32_t data) {
    CAN.beginPacket(0x12);
    for (int i = 3; i >= 0; i--) CAN.write((data >> (i * 8)) & 0xFF);
    CAN.endPacket();
}

uint32_t read() {
    uint32_t message = 0;
    while (CAN.available()) {
        message = (message << 8) | CAN.read();
    }
    return message;
}