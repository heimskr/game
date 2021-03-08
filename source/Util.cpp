#include <stdexcept>
#include <vector>

#include "Util.h"

PrintConsole *console = nullptr;
std::vector<PrintConsole> savedConsoles;

void saveConsole() {
	if (console)
		savedConsoles.push_back(*console);
}

void restoreConsole() {
	if (console && !savedConsoles.empty()) {
		*console = savedConsoles.back();
		savedConsoles.pop_back();
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

std::vector<std::string> split(const std::string &str, const std::string &delimiter, bool condense) {
	if (str.empty())
		return {};

	size_t next = str.find(delimiter);
	if (next == std::string::npos)
		return {str};

	std::vector<std::string> out {};
	const size_t delimiter_length = delimiter.size();
	size_t last = 0;

	out.push_back(str.substr(0, next));

	while (next != std::string::npos) {
		last = next;
		next = str.find(delimiter, last + delimiter_length);
		std::string sub = str.substr(last + delimiter_length, next - last - delimiter_length);
		if (!sub.empty() || !condense)
			out.push_back(std::move(sub));
	}

	return out;
}

long parseLong(const std::string &str, int base) {
	const char *c_str = str.c_str();
	char *end;
	long parsed = strtol(c_str, &end, base);
	if (c_str + str.length() != end)
		throw std::invalid_argument("Not an integer: \"" + str + "\"");
	return parsed;
}

long parseLong(const std::string *str, int base) {
	return parseLong(*str, base);
}

long parseLong(const char *str, int base) {
	return parseLong(std::string(str), base);
}

double parseDouble(const std::string &str) {
	const char *c_str = str.c_str();
	char *end;
	double parsed = strtod(c_str, &end);
	if (c_str + str.length() != end)
		throw std::invalid_argument("Not an integer: \"" + str + "\"");
	return parsed;
}

double parseDouble(const std::string *str) {
	return parseDouble(*str);
}

double parseDouble(const char *str) {
	return parseDouble(std::string(str));
}

unsigned long parseUlong(const std::string &str, int base) {
	const char *c_str = str.c_str();
	char *end;
	unsigned long parsed = strtoul(c_str, &end, base);
	if (c_str + str.length() != end)
		throw std::invalid_argument("Not an integer: \"" + str + "\"");
	return parsed;
}

unsigned long parseUlong(const std::string *str, int base) {
	return parseUlong(*str, base);
}

unsigned long parseUlong(const char *str, int base) {
	return parseUlong(std::string(str), base);
}
