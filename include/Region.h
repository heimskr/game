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
		std::set<Area> areas;
		size_t money = 0;

		Region(size_t size_);

		Resource::Map allResources() const;
};
