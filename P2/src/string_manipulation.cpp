#include "string_manipulation.h"

int separate(String command, String separatedCommand[], int max) {
  int index = 0;

  command.trim();
  while (command.length() > 0 && index < max) {
    int pos = command.indexOf(' ');

    if (pos == -1) {
      separatedCommand[index++] = command;
      command = "";
    } else {
      separatedCommand[index++] = command.substring(0, pos);
      command = command.substring(pos + 1);
      command.trim();
    }
  }

  return index+1;
}

bool isNumber(const String &token) {
  if (token.length() == 0) return false;

  for (unsigned int i = 0; i < token.length(); i++) {
    if (!isDigit(token[i])) return false;
  }
  return true;
}