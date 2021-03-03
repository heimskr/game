#pragma once

#include <utility>
#include <switch.h>

#include "Logger.h"

extern PrintConsole *console;
extern int oldConsoleWidth, oldConsoleHeight;

void saveConsole();
void restoreConsole();
void defaultConsole();

template <typename ...Args>
void print(Args &&...args) {
	printf(std::forward<Args>(args)...);
	consoleUpdate(console);
}
