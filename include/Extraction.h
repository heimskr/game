#pragma once

#include <string>

class Area;
class Game;

struct Extraction {
	Area *area;
	std::string resourceName;
	double startAmount;
	double amount;
	double rate;

	Extraction(Area *area_, const std::string &resource_name, double amount_, double rate_):
		area(area_), resourceName(resource_name), startAmount(amount_), amount(amount_), rate(rate_) {}

	Extraction(Area *area_, const std::string &resource_name, double start_amount, double amount_, double rate_):
		area(area_), resourceName(resource_name), startAmount(start_amount), amount(amount_), rate(rate_) {}

	std::string toString() const;
	static Extraction fromString(const Game &, const std::string &);
};
