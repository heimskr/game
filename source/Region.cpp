#include "Region.h"

Region::Region(size_t size_): size(size_) {}

Resource::Map Region::allResources() const {
	Resource::Map out;
	for (const Area &area: areas)
		for (const Resource::Map::value_type &pair: area.resources)
			out[pair.first] += pair.second;
	return out;
}
