#pragma once

#include <string>
#include <set>

#include <stddef.h>

#include "Area.h"
#include "Resource.h"

/** Regions are individual locations within the broader game universe. It's yet to be determined whether they're
 *  islands, planets or something else. Regions are divided into Areas. A Region can have only one Area of a given type.
 */
class Region {
	public:
		size_t size = 0;
		/** Don't directly insert Areas into this map; use operator+= instead. */
		std::unordered_map<std::string, Area> areas;
		size_t money = 0;

		Region(size_t size_);

		Resource::Map allResources() const;
		size_t totalPopulation() const;

		Region & operator+=(Area);
};
