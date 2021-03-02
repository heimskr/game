#include "Resource.h"

Resource::Resource(const Name &name_): name(name_) {}
Resource::Resource(const char *name_): name(name_) {}

Resource & Resource::add(const Name &name_, const Conversion &conversion) {
	conversions.emplace(name_, conversion);
	return *this;
}
