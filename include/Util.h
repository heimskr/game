#pragma once

#include <map>
#include <sstream>
#include <string>
#include <switch.h>
#include <utility>
#include <vector>

#include "Logger.h"

#define PRINTPOS do { fprintf(stderr, "%s:%d\n", __FILE__, __LINE__); fflush(stderr); } while(0)

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
bool chance(double probability);
size_t randomRange(size_t min, size_t max);
double randomRangeDouble(double min, double max);
std::string stringify(const std::map<std::string, double> &);
std::map<std::string, double> parseMap(const std::string &);
bool contains(const std::map<std::string, double> &, const std::map<std::string, double> &);
void shrink(std::map<std::string, double> &);
