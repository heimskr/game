#pragma once

#include <string>

class Game;
class Processor;

/** An automation link connects one processor's output to another processor's input for one type of resource. */
struct AutomationLink {
	Game *game;
	Processor *producer = nullptr, *consumer = nullptr;
	std::string resourceName;
	double weight = 1.;

	AutomationLink(Game &, Processor *producer_, Processor *consumer_, const std::string &resource_name, double weight_);
	AutomationLink(Game &, const std::string &);

	std::string toString() const;
	void tick();
};
