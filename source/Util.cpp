#include <list>
#include <stdexcept>

#include "Resource.h"
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

bool chance(double probability) {
	return ((rand() % 100000000) / 100000000.) < probability;
}

size_t randomRange(size_t min, size_t max) {
	if (max <= min)
		return min;
	return min + (rand() % (max - min + 1));
}

double randomRangeDouble(double min, double max) {
	if (max <= min)
		return min;
	return min + static_cast<double>(rand()) / RAND_MAX * (max - min);
}

std::string stringify(const std::map<std::string, double> &map) {
	std::stringstream out;
	bool first = true;
	for (const auto &[name, amount]: map) {
		if (first)
			first = false;
		else
			out << "/";
		out << name << "/" << amount;
	}
	return out.str();
}

std::map<std::string, double> parseMap(const std::string &str) {
	const std::vector<std::string> pieces = split(str, "/", false);
	std::map<std::string, double> out;
	for (size_t i = 0, max = pieces.size(); i < max; i += 2)
		out.emplace(pieces[i], parseDouble(pieces[i + 1]));
	return out;
}

bool contains(const std::map<std::string, double> &left, const std::map<std::string, double> &right) {
	for (const auto &[name, amount]: right)
		if (left.count(name) == 0 || left.at(name) < amount)
			return false;
	return true;
}

void shrink(Resource::Map &map) {
	std::list<const std::string *> to_remove;
	for (const auto &[name, amount]: map)
		if (amount < Resource::MIN_AMOUNT)
			to_remove.push_back(&name);
	for (const std::string *name: to_remove)
		map.erase(*name);
}

void shrink(Resource::Map &map, const std::string &key) {
	if (map[key] < Resource::MIN_AMOUNT)
		map.erase(key);
}
