#pragma once

#include <map>
#include <string>

#include <stddef.h>

#include "resource/Resource.h"

class Region {
	public:
		size_t area = 0;
		std::map<Resource, size_t> resources;
		size_t money = 0;

		Region(size_t area_);
};
