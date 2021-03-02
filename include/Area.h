#pragma once

#include <cstddef>
#include <string>
#include <unordered_map>

#include "Resource.h"

class Region;

class Area {
	public:
		using Name = std::string;

		Region *parent;
		Resource::Map resources;

		Area() = delete;
		Area(Region *);

		Area & set(const Resource::Map &);
};
