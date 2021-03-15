#pragma once

#include "Area.h"

class OilFieldArea: public Area {
	public:
		constexpr static double OIL_MAX = 10.;

		OilFieldArea(Region *, size_t = 0);
		Type getType() const override { return Type::OilField; }
		std::string description() const override;
		void tick(double delta) override;
};
