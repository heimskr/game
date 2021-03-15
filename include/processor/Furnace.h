#include "Processor.h"

class Furnace: public Processor {
	public:
		double fuel = 0.;

		using Processor::Processor;

		Furnace & setFuel(double);

		std::string toString() const override;
		double tick(double delta) override;
		Type getType() const override { return Type::Furnace; }

	protected:
		void headerAdditional(Context &, long index) override;
		void headerButtons(Context &, long index) override;
};
