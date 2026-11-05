#include <Arduino.h>
#include "string_manipulation.h"
#include "printer_manager.h"
#include "can_communications.h"
#include "token_classifyer.h"

#define MAX_TOKENS 4

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
  }
  if (command == "help") {
    printHelpCommand();
  } else if (command == "us") {
    sendUsCommand(command);
    readResponse(read());
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

void readResponse(uint32_t message) {
  String operation = ((u_int8_t)(message >> 24) == 1) ? "us" : "help";
  uint8_t sensor = (message >> 16) & 0xFF;
  uint8_t error = (message >> 11) & 0x1;
  String firstParameter = getFirstParameter(message >> 12 & 0xF);
  String secondParameter = getSecondParameter(message >> 8 & 0x7);

  String data = operation + " " + firstParameter + " " + secondParameter;
  if (error){
    Serial.println(data + "went wrong on sensor: " + sensor);
    printErrorMessage(message & 0xF);
  } else {
    Serial.println(data + "succesfully executed on sensor " + sensor);
  }
}

