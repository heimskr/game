#pragma once

#include "Area.h"

class MountainArea: public Area {
	public:
		constexpr static double COAL_MAX = 50.;
		constexpr static double IRON_MAX = 50.;
		constexpr static double STONE_MAX = 200.;

		MountainArea(Region *, size_t = 0);
		Type getType() const override { return Type::Mountain; }
		std::string description() const override;
		void tick(double delta) override;
};
