#pragma once

#include "Area.h"

class LakeArea: public Area {
	private:
		static constexpr double WATER_MAX = 25.;
		static constexpr double FISH_DENSITY = 10.;

	public:
		LakeArea(Region *, size_t = 0);
		Type getType() const override { return Type::Lake; }
		std::string description() const override;
		void tick(double delta) override;
};
