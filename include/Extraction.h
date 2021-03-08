#pragma once

#include <string>

class Area;

struct Extraction {
	Area *area;
	std::string resourceName;
	double amount;
	double rate;

	Extraction(Area *area_, const std::string &resource_name, double amount_, double rate_):
		area(area_), resourceName(resource_name), amount(amount_), rate(rate_) {}
};
