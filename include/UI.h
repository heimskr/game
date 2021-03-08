#pragma once

#include <stdio.h>
#include <switch/runtime/devices/console.h>

void leftPanel(PrintConsole *, int width);
void rightPanel(PrintConsole *, int width);
void topPanel(PrintConsole *, int height);
void bottomPanel(PrintConsole *, int height);
void verticalPanel(PrintConsole *, int y, int height);
void resetWindow(PrintConsole *);

void onTouch(int x, int y);
void onTouchUp(int x, int y);
void drawUI();
