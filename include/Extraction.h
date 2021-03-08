#pragma once

#include <string>

class Area;
class Game;

struct Extraction {
	Area *area;
	std::string resourceName;
	double amount;
	double rate;

	Extraction(Area *area_, const std::string &resource_name, double amount_, double rate_):
		area(area_), resourceName(resource_name), amount(amount_), rate(rate_) {}

	std::string toString() const;
	static Extraction fromString(const Game &, const std::string &);
};
