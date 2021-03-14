#include "Processor.h"

struct Centrifuge: public Processor {
	using Processor::Processor;
	double tick(double delta) override;
	Type getType() const override { return Type::Centrifuge; }
	void headerButtons(Context &, long index) override;
};
