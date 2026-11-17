#include <Arduino.h>
#include "revisor.h"
#include "frame_decoder.h"
#include "response_queue.h"
#include "string_manipulation.h"
#include "printer_manager.h"
#include "can_communications.h"
#include "token_classifyer.h"

#define MAX_TOKENS 4

static String lineBuffer;

bool userIsTyping() {
  return lineBuffer.length() > 0;
}

bool readKeyboard(String &line) {
  while (Serial.available()) {
    char c = Serial.read();

    if (c == '\r') continue;

    if (c == '\n') {
      line = lineBuffer;
      line.trim();
      lineBuffer = "";
      Serial.println();
      return true;
    }

    if (c == 0x08 || c == 0x7F) {
      if (lineBuffer.length() > 0) {
        lineBuffer.remove(lineBuffer.length() - 1);
        Serial.print("\b \b");
      }
      continue;
    }

    lineBuffer += c;
    Serial.print(c);
  }
  return false;
}

bool classifyToken(const String &token, int index, uint32_t &message) {
    switch (index) {
        case 0:
            if (token != "us") return false;
            message |= getOperationValue(token);
            return true;

        case 1: {
            if (!isNumber(token)) return false;
            long sensor = token.toInt();
            if (sensor < 0 || sensor > 255) return false;
            message |= getSensorValue((uint8_t)sensor);
            return true;
        }

        case 2: {
            bool valid = token == "one-shot" || token == "on" || token == "off" ||
                        token == "unit" || token == "delay" || token == "status";
            if (!valid) return false;
            message |= getFirstParameterValue(token);
            return true;
        }

        case 3: {
            String first = getFirstParameter((message >> 12) & 0xF);

            if (first == "on" || first == "delay") {
                if (!isNumber(token)) return false;
                long value = token.toInt();
                if (value < 0 || value > 255) return false;
                message |= getValueField((uint8_t)value);
                return true;
            }
            if (first == "unit") {
                if (token != "inc" && token != "cm" && token != "ms") return false;
                message |= getSecondParameterValue(token);
                return true;
            }
            return false;
        }
    }
    return false;
}

static int expectedTokens(uint32_t message) {
    String first = getFirstParameter((message >> 12) & 0xF);
    if (first == "on" || first == "delay" || first == "unit") return 4;
    return 3;
}

void sendUsCommand(String command) {
    command.trim();
    String separatedCommand[MAX_TOKENS];
    int numberOfTokens = separate(command, separatedCommand, MAX_TOKENS);
    uint32_t message = 0;

    for (int index = 0; index < numberOfTokens; index++) {
        if (!classifyToken(separatedCommand[index], index, message)) {
        printErrorMessage(0);
        return;
        }
    }

    if (numberOfTokens != expectedTokens(message)) {
        printErrorMessage(0);
        return;
    }

    send(message);
}

void processCommand(String command) {
    if (command == "") {
        return;
    }
    if (command == "help") {
        printHelpCommand();
    } else if (command == "us" || command.startsWith("us ")) {
        sendUsCommand(command);
    } else {
        printErrorMessage(0);
    }
}

void printPendingResponses() {
    CanPacket packet;
    while (popPacket(packet)) {
        readResponse(packet);
    }

    uint16_t dropped = takeDroppedMessages();
    if (dropped) {
        Serial.println("Aviso: se perdieron " + String(dropped) + " paquetes CAN");
    }
}