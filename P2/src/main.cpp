#include <CAN.h>
#include "revisor.h"
#include "printer_manager.h"

#define REVISOR true

void setup() {
  Serial.begin(9600);
  while (!Serial);
  prepareShell();

  // start the CAN bus at 500 kbps
  if (!CAN.begin(500E3)) {
    Serial.println("Starting CAN failed!");
    while (1);
  }
}

void loop() {
  #if REVISOR
    String command = readKeyboard();
    if (command.length() == 0) return;

    proccessCommand(command);
    prepareShell();
  #else
  #endif
}
