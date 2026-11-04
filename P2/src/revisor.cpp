#include <Arduino.h>
#include "string_manipulation.h"
#include "message_printer.h"
#include "can_communications.h"

#define MAX_TOKENS 4

void prepareShell() {
  Serial.print("$ ");
}

String readKeyboard() {
  if(!Serial.available()) {
    return String();
  }
  String command = Serial.readStringUntil('\n');
  command.trim();
  return command;
}

void proccessCommand(String command) {
  if (command == "") {
    Serial.println();
    prepareShell();
  }
  if (command == "help") {
    printHelpCommand();
  } else if (command == "us") {
    sendUsCommand(command);
  } else{
    printErrorMessage(0);
  }
}

void sendUsCommand(String command){
  command.trim();
  String separatedCommand[MAX_TOKENS];
  int numberOfTokens = separate(command, separatedCommand, MAX_TOKENS);
  uint32_t message;
  bool messageComplete = true;

  for (int index=0; index < numberOfTokens; index++){
    if(!classifyToken(separatedCommand[index], index, message)) {
      printErrorMessage(0);
      messageComplete = false;
      break;
    }
  }

  if (messageComplete) send(message);
}

uint32_t classifyToken(const String &token, int index, uint32_t message){
  // Operation code
  if (index == 0){
    if (token != "us") return false;
    message |= 8UL << 12;
    return true;
  }

  // First parametre
  bool firstParametre = token == "one-shot" || token == "on" || token == "off" || token == "unit" || token == "delay" || token == "status";
  if (index == 2){
    if (!firstParametre) return false;
    if (token == "one-shot") message |= 8UL << 12;
    else if (token == "on") message |= 1UL << 12;
    else if (token == "off") message |= 14UL << 12;
    else if (token == "unit") message |= 7UL << 12;
    else if (token == "delay") message |= 4UL << 12;
    else if (token == "status") message |= 13UL << 12;
    return true;
  }

  // Second parametre
  bool secondParametre = token == "ms" || token == "inc" || token == "cm";
  if (index == 3) {
    if (!isNumber(token) && !secondParametre) return false;
    if (token == "inc") message |= 1UL << 7;
    else if (token == "cm") message |= 2UL << 7;
    else if (token == "ms") message |= 3UL << 7;
  
    // Period/delay in ms
    else if (isNumber(token)) message |= (uint32_t)token.toInt();

    return true;
  }
  
  return false;
}