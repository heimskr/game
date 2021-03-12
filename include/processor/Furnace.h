#include "Processor.h"

struct Furnace: public Processor {
	double fuel = 0.;

	Furnace(Game &, double fuel_, const std::map<std::string, double> &input_, const std::map<std::string, double> &output_);
	Furnace(Game &);

	virtual std::string toString() const override;
	double tick(double delta) override;
	Type getType() const override { return Type::Furnace; }
};
