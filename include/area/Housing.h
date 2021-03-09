#pragma once

#include "Area.h"

class HousingArea: public Area {
	public:
		HousingArea(Region *, size_t = 0);
		Type getType() const override { return Type::Housing; }
		std::string description() const override;
		void tick(double delta) override;
};
