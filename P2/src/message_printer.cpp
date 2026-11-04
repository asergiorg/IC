#include "message_printer.h"

void prepareShell() {
  Serial.print("$ ");
}

void printHelpCommand(){
  Serial.println();
  Serial.println("Available commands:");
  Serial.println("  help - show this help message");
  Serial.println("  us <srf02> one-shot - produce one shot of the ultrasound sensor specified");
  Serial.println("  us <srf02> on <period_ms> - produce periodic shots of the ultrasound sensor specified");
  Serial.println("  us <srf02> off - stops any periodic shot of the ultrasound sensor specified");
  Serial.println("  us <srf02> unit {inc | cm | ms} - change the unit of the measure a sensor return");
  Serial.println("  us <srf02> delay <ms> - modify the minimun delay between consecutive shots of the srf02 sensor");
  Serial.println("  us <srf02> status - return the status ");
  Serial.println("  us - show the relation between the available ultrasound sensors");
  Serial.println();
}

void printErrorMessage(int errorId){
  switch (errorId) {
  case 0:
      Serial.println("Command not fount.");
      Serial.println("Type \"help\" to learn more about available commands.");
    break;
  
  default:
    break;
  }
}