#pragma once

#include <memory>
#include <string>

#include "Resource.h"

struct Processor {
	enum class Type {Furnace, Centrifuge};

	Type type;
	Resource::Map input, output;

	Processor(Type type_, const Resource::Map &input_, const Resource::Map &output_);

	virtual std::string toString() const;
	virtual void tick() = 0;
	static Processor * fromString(const std::string &);
};
