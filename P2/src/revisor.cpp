#include <Arduino.h>

void prepareShell() {
  Serial.print("  ");
}

char *readKeyboard(void) {
  size_t size = 16;
  int numberOfChars = 0;
  char *command = (char *)malloc(size + 1);

  while(true){

    if (numberOfChars == size){
      size *= 2;
      command = (char *)realloc(command, size + 1);
    }
  }


  return command;
}