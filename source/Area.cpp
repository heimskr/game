#include "Area.h"

Area::Area(Region *parent_): parent(parent_) {}

Area & Area::set(const Resource::Map &resources_) {
	resources = resources_;
	return *this;
}
