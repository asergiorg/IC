// Copyright (c) Sandeep Mistry. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <Arduino.h>
#include <CAN.h>
#include "revisor.h"

#define REVISOR true

void setup() {
  Serial.begin(9600);
  while (!Serial);

  // start the CAN bus at 500 kbps
  if (!CAN.begin(500E3)) {
    Serial.println("Starting CAN failed!");
    while (1);
  }
}

void loop() {
  #if REVISOR
    prepareShell();
    String command = readKeyboard();
    proccessCommand(command);

  #else
  #endif
}
