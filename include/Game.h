#pragma once

#include <map>

#include "Resource.h"
#include "Util.h"

namespace Globals {
	extern bool done;
	extern std::map<Resource::Name, Resource> resources;

	void add(const Resource &);

	void addResources();
	void addAll();

	void addResource(const Resource::Name &);
	
}

class Game {
	public:
		Game();
};