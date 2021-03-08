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
