#include "revisor.h"
#include "printer_manager.h"
#include "can_communications.h"

#define REVISOR true

void setup() {
  Serial.begin(9600);
  while (!Serial);

  canBegin();
  Serial.print("> ");
}

void loop() {
  #if REVISOR
    // Solo mostramos respuestas CAN si el usuario no está escribiendo
    if (!userIsTyping()) {
      printPendingResponses();
    }

    String line;
    if (readKeyboard(line)) {
      processCommand(line);
      Serial.print("> ");
    }
  #else
  #endif
}
