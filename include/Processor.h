#pragma once

#include <map>
#include <memory>
#include <string>

class Game;

// TODO: probably not make subclasses of this
struct Processor {
	enum class Type {Furnace, Centrifuge};

	Game *owner;
	std::map<std::string, double> input, output;

	Processor(Game &, const std::map<std::string, double> &input_, const std::map<std::string, double> &output_);
	Processor(Game &);

	virtual std::string toString() const;
	/** Returns the amount processed. */
	virtual double tick();
	virtual Type getType() const = 0;
	static Processor * fromString(Game &, const std::string &);
	static const char * typeName(Type);
};
