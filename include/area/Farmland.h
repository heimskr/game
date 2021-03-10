#pragma once

#include "Area.h"

class FarmlandArea: public Area {
	private:
		static constexpr double FOOD_MAX = 4.0;

	public:
		std::string resourceName;
		FarmlandArea(Region *, size_t, const std::string &resource_name);
		FarmlandArea(Region *, size_t = 0);
		Type getType() const override { return Type::Farmland; }
		std::string description() const override;
		void tick(double delta) override;
		std::string toString() const override;
};
