#ifndef FRAME_DECODER_H
#define FRAME_DECODER_H

#include <Arduino.h>
#include "response_queue.h"

void readResponse(const CanPacket &packet);
void resetFrameState();
bool frameInProgress();

#endif