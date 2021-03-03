#include "Util.h"

PrintConsole *console = nullptr;
int oldConsoleWidth = 80;
int oldConsoleHeight = 45;

void saveConsole() {
	if (console) {
		oldConsoleWidth = console->consoleWidth;
		oldConsoleHeight = console->consoleHeight;
	}
}

void restoreConsole() {
	if (console) {
		console->consoleWidth = oldConsoleWidth;
		console->consoleHeight = oldConsoleHeight;
		consoleInit(console);
	}
}

void defaultConsole() {
	if (console) {
		saveConsole();
		console->consoleWidth = 80;
		console->consoleHeight = 45;
		consoleSelect(console);
	}
}
