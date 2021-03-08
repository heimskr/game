#include <switch.h>

#include "UI.h"
#include "Util.h"

Sidebar sidebar = {30, 4};

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
	if (sidebar.visible && console->consoleWidth - sidebar.width <= x) {
		if (y < sidebar.buttonHeight)
			sidebar.up();
		else if (console->consoleHeight - sidebar.buttonHeight <= y)
			sidebar.down();
	}
}

Sidebar::Sidebar(int width_, int button_height): width(width_), buttonHeight(button_height) {}

void Sidebar::draw() {
	if (visible) {
		saveConsole();
		console->cursorX = console->cursorY = 0;
		rightPanel(console, width);
		topPanel(console, buttonHeight);
		console->bg = 23;
		print("\e[2J");
		bottomPanel(console, buttonHeight);
		print("\e[2J");
		verticalPanel(console, buttonHeight, console->consoleHeight - 2 * buttonHeight);
		print("\e[47m\e[2J\e[0m");
		resetWindow(console);
		restoreConsole();
		leftPanel(console, console->consoleWidth - width);
		if (console->windowWidth <= console->cursorX) {
			console->cursorX = console->windowWidth - 1;
		}
		if (console->windowHeight <= console->cursorY) {
			console->cursorY = console->windowHeight - 1;
		}
	} else {
		resetWindow(console);
	}
}

void Sidebar::up() {
	saveConsole();
	console->cursorX = console->cursorY = 0;
	verticalPanel(console, buttonHeight, console->consoleHeight - 2 * buttonHeight);
	rightPanel(console, width);
	print("\e[30;47mUp  \n");
	restoreConsole();
}

void Sidebar::down() {
	saveConsole();
	console->cursorX = console->cursorY = 0;
	verticalPanel(console, buttonHeight, console->consoleHeight - 2 * buttonHeight);
	rightPanel(console, width);
	print("\e[30;47mDown\n");
	restoreConsole();
}
