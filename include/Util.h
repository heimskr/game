#pragma once

#include <sstream>
#include <string>
#include <switch.h>
#include <utility>
#include <vector>

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

template <typename T>
std::string hex(T number) {
	std::stringstream ss;
	ss << std::hex << number;
	return ss.str();
}

std::vector<std::string> split(const std::string &str, const std::string &delimiter, bool condense = true);
long parseLong(const std::string &, int base = 10);
long parseLong(const std::string *, int base = 10);
long parseLong(const char *, int base = 10);
double parseDouble(const std::string &);
double parseDouble(const std::string *);
double parseDouble(const char *);
unsigned long parseUlong(const std::string &, int base = 10);
unsigned long parseUlong(const std::string *, int base = 10);
unsigned long parseUlong(const char *, int base = 10);
