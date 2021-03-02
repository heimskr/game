#pragma once

#include <cstddef>
#include <string>
#include <unordered_map>

#include "Resource.h"

class Region;

class Area {
	public:
		Region *parent;
		Resource::Map resources;
		size_t size;
		std::string name;

		Area() = delete;
		Area(Region *, size_t = 0);

		Area & set(const Resource::Map &);
		Area & set(size_t);

		size_t totalPopulation() const;
};
