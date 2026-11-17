#include <Arduino.h>
#include "revisor.h"
#include "string_manipulation.h"
#include "printer_manager.h"
#include "can_communications.h"
#include "token_classifyer.h"
#include "response_queue.h"

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
  // Operation code
  if (index == 0){
    if (token != "us") return false;
    message |= 8UL << 12;
    return true;
  }

  // First parameter
  bool firstParameter = token == "one-shot" || token == "on" || token == "off" || token == "unit" || token == "delay" || token == "status";
  if (index == 2){
    if (!firstParameter) return false;
    message |= getFirstParameterValue(token);
    return true;
  }

  // Second parameter
  bool secondParameter = token == "ms" || token == "inc" || token == "cm";
  if (index == 3) {
    if (!isNumber(token) && !secondParameter) return false;
    message |= getSecondParameterValue(token);
    return true;
  }

  return false;
}

void sendUsCommand(String command){
  command.trim();
  String separatedCommand[MAX_TOKENS];
  int numberOfTokens = separate(command, separatedCommand, MAX_TOKENS);
  uint32_t message = 0;

  for (int index=0; index < numberOfTokens; index++){
    if(!classifyToken(separatedCommand[index], index, message)) {
      printErrorMessage(0);
      break;
    }
  }
}

void processCommand(String command) {
  if (command == "") {
    return;
  }
  if (command == "help") {
    printHelpCommand();
  } else if (command.startsWith("us")) {
    sendUsCommand(command);
  } else {
    printErrorMessage(0);
  }
}

void readResponse(uint32_t message) {
  String operation = ((uint8_t)(message >> 24) == 1) ? "us" : "help";
  uint8_t sensor = (message >> 16) & 0xFF;
  uint8_t error = (message >> 11) & 0x1;
  String firstParameter = getFirstParameter(message >> 12 & 0xF);
  String secondParameter = getSecondParameter(message >> 8 & 0x7);

  String decodification = operation + " " + firstParameter + " " + secondParameter;
  if (error){
    Serial.println(decodification + " went wrong on sensor " + String(sensor));
    printErrorMessage(message & 0xF);
  } else {
    Serial.println(decodification + " successfully executed on sensor " + String(sensor));
  }
}

void printPendingResponses() {
  uint32_t msg;
  while (popResponse(msg)) {
    readResponse(msg);
  }

  uint16_t dropped = takeDroppedMessages();
  if (dropped) {
    Serial.println("Aviso: se perdieron " + String(dropped) + " mensajes CAN");
  }
}