#pragma once

#include <array>
#include <map>
#include <memory>
#include <string>

class Context;
class Game;

class Processor {
	public:
		enum class Type {Furnace, Centrifuge, Fermenter};
		static constexpr auto TYPES = std::array {Type::Furnace, Type::Centrifuge, Type::Fermenter};

		Game *game;
		std::map<std::string, double> input, output;
		bool autoExtract = false;

		Processor(Game &, const std::map<std::string, double> &input_, const std::map<std::string, double> &output_, bool auto_extract);
		Processor(Game &);

		virtual std::string toString() const;
		/** Returns the amount processed. */
		virtual double tick(double delta);
		void moveOutput();
		virtual void renderHeader(Context &, long index);
		virtual void renderBody(Context &, long index);
		virtual std::string getName() const;
		virtual Type getType() const = 0;
		static Processor * fromString(Game &, const std::string &);
		static Processor * ofType(Game &, Processor::Type);
		static const char * typeName(Type);

	protected:
		virtual void headerAdditional(Context &, long index);
		virtual void headerButtons(Context &, long index);
};
