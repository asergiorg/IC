#ifndef CAN_COMMUNICATIONS_H
#define CAN_COMMUNICATIONS_H

#include <Arduino.h>
#include <CAN.h>

void send(uint32_t message);
uint32_t read();

#endif