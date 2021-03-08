#pragma once

#include <stdio.h>
#include <switch/runtime/devices/console.h>

constexpr int SIDEBAR_WIDTH = 30;
constexpr int BUTTON_HEIGHT = 4;

void leftPanel(PrintConsole *, int width);
void rightPanel(PrintConsole *, int width);
void topPanel(PrintConsole *, int height);
void bottomPanel(PrintConsole *, int height);
void verticalPanel(PrintConsole *, int y, int height);
void resetWindow(PrintConsole *);

void onTouch(int x, int y);
void onTouchUp(int x, int y);

struct Sidebar {
	int width = 30;
	int buttonHeight = 4;
	bool visible = true;
	Sidebar(int width_, int button_height);
	void draw();
	void up();
	void down();
};

extern Sidebar sidebar;
