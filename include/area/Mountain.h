#pragma once

#include "Area.h"

class MountainArea: public Area {
	public:
		constexpr static double COAL_MAX = 100.;
		constexpr static double IRON_MAX = 100.;

		MountainArea(Region *, size_t = 0);
		Type getType() const override { return Type::Mountain; }
		std::string description() const override;
		void tick() override;
};
