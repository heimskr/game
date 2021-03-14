#include "Processor.h"

class Furnace: public Processor {
	public:
		double fuel = 0.;

		Furnace(Game &, double fuel_, const std::map<std::string, double> &input_, const std::map<std::string, double> &output_, bool auto_extract);
		Furnace(Game &);

		virtual std::string toString() const override;
		double tick(double delta) override;
		Type getType() const override { return Type::Furnace; }

	protected:
		void headerAdditional(Context &, long index) override;
};
