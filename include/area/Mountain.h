#pragma once

#include "Area.h"

class MountainArea: public Area {
	public:
		MountainArea(Region *, size_t = 0);
		Type getType() const override { return Type::Mountain; }
		std::string description() const override;
		void tick() override;
};
