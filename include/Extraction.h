#pragma once

#include <string>

class Area;

struct Extraction {
	Area *area;
	std::string resourceName;
	double rate;

	Extraction(Area *area_, const std::string &resource_name, double rate_):
		area(area_), resourceName(resource_name), rate(rate_) {}
};
