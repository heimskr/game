#include <switch.h>

#include "UI.h"
#include "Util.h"

void leftPanel(PrintConsole *pc, int width) {
	pc->windowWidth = width;
	pc->windowX = 0;
}

void rightPanel(PrintConsole *pc, int width) {
	pc->windowWidth = width;
	pc->windowX = pc->consoleWidth - width;
}

void topPanel(PrintConsole *pc, int height) {
	pc->windowHeight = height;
	pc->windowY = 0;
}

void bottomPanel(PrintConsole *pc, int height) {
	pc->windowHeight = height;
	pc->windowY = pc->consoleHeight - height;
}

void verticalPanel(PrintConsole *pc, int y, int height) {
	pc->windowY = y;
	pc->windowHeight = height;
}

void resetWindow(PrintConsole *pc) {
	leftPanel(pc, pc->consoleWidth);
	topPanel(pc, pc->consoleHeight);
}	

void onTouch(int x, int y) {
	print("\e[%d;%dH\e[42;31mx\e[39;40m", y + 1, x + 1);
}

void onTouchUp(int x, int y) {
	print("\e[%d;%dH\e[41;32m!\e[39;40m", y + 1, x + 1);
}

void drawUI() {
	constexpr int SIZE = 4;
	constexpr int WIDTH = 30;
	rightPanel(console, WIDTH);
	topPanel(console, SIZE);
	console->bg = 23;
	print("\e[2J");
	bottomPanel(console, SIZE);
	print("\e[2J");
	verticalPanel(console, SIZE, 45 - 2 * SIZE);
	print("\e[47m\e[2J\e[0m");
	resetWindow(console);
	leftPanel(console, console->consoleWidth - WIDTH);
}
