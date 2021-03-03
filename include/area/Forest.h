#pragma once

#include "Area.h"

class ForestArea: public Area {
	public:
		ForestArea(Region *, size_t = 0);
		Type getType() const override { return Type::Forest; }
		void tick() override;
};
