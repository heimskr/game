#include "Processor.h"

class RocketFurnace: public Processor {
	public:
		double hydrogen = 0., oxygen = 0.;

		using Processor::Processor;

		RocketFurnace & setHydrogen(double);
		RocketFurnace & setOxygen(double);

		std::string toString() const override;
		double tick(double delta) override;
		Type getType() const override { return Type::RocketFurnace; }

	protected:
		void headerAdditional(Context &, long index) override;
		void headerButtons(Context &, long index) override;
};
