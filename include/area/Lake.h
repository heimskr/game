#pragma once

#include "Area.h"

class LakeArea: public Area {
	private:
		static constexpr double WATER_MAX = 4.0;

	public:
		LakeArea(Region *, size_t = 0);
		Type getType() const override { return Type::Lake; }
		std::string description() const override;
		void tick() override;
};
