#pragma once

#include <memory>
#include <string>

class Game;
class Processor;

/** An automation link connects one processor's output to another processor's input for one type of resource. */
struct AutomationLink {
	Game *game;
	std::shared_ptr<Processor> producer, consumer;
	std::string resourceName;
	double weight = 1.;

	AutomationLink(Game &, std::shared_ptr<Processor> producer_, std::shared_ptr<Processor> consumer_, const std::string &resource_name, double weight_);
	AutomationLink(Game &, const std::string &);

	std::string toString() const;
	void tick();
};
