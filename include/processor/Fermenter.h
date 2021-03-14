#include "Processor.h"

class Fermenter: public Processor {
	public:
		double yeast = 0.;

		using Processor::Processor;

		Fermenter & setYeast(double);

		virtual std::string toString() const override;
		double tick(double delta) override;
		Type getType() const override { return Type::Fermenter; }

	protected:
		void headerAdditional(Context &, long index) override;
		void headerButtons(Context &, long index) override;
};
