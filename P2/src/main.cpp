#include "revisor.h"
#include "printer_manager.h"
#include "can_communications.h"

#define REVISOR true

void setup() {
  Serial.begin(9600);
  while (!Serial);

  canBegin();
  #if REVISOR
    requestPrompt(0);
  #endif
}

void loop() {
  #if REVISOR
    if (!userIsTyping()) {
      printPendingResponses();
    }

    String line;
    if (readKeyboard(line)) {
      bool sent = processCommand(line);
      requestPrompt(sent ? RESPONSE_TIMEOUT_MS : 0);
    }

    updatePrompt();
  #else
  #endif
}