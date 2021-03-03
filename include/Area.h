#pragma once

#include <cstddef>
#include <string>
#include <unordered_map>

#include "Resource.h"

class Region;

class Area {
	public:
		static constexpr const char *INVALID_CHARS = ";";

		enum class Type {Housing, Forest};

		Region *parent;
		Resource::Map resources;
		size_t size;
		std::string name;
		bool playerOwned = false;

		Area() = delete;
		Area(Region *, size_t = 0);

		Area & setResources(const Resource::Map &);
		Area & setSize(size_t);
		Area & setPlayerOwned(bool);

		size_t totalPopulation() const;

		virtual Type getType() const = 0;
		virtual std::string description() const = 0;
		virtual void tick() = 0;
};
