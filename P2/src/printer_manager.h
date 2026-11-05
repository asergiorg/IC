#ifndef PRINTER_MANAGER_H
#define PRINTER_MANAGER_H

#include <Arduino.h>

void prepareShell();
void printHelpCommand();
void printErrorMessage(int errorId);

#endif