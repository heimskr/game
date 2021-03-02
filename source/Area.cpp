#include "Area.h"

Area::Area(Region *parent_, size_t size_): parent(parent_), size(size_) {}

Area & Area::set(const Resource::Map &resources_) {
	resources = resources_;
	return *this;
}

Area & Area::set(size_t size_) {
	size = size_;
	return *this;
}

size_t Area::totalPopulation() const {
	size_t out = 0;
	for (const auto &pair: resources)
		if (Resource::hasType(pair.first, "sapient"))
			out += static_cast<size_t>(pair.second);
	return out;
}
