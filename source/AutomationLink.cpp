#include "AutomationLink.h"
#include "Game.h"
#include "Processor.h"
#include "Util.h"

AutomationLink::AutomationLink(Game &game_, std::shared_ptr<Processor> producer_, std::shared_ptr<Processor> consumer_,
const std::string &resource_name, double weight_):
	game(&game_), producer(producer_), consumer(consumer_), resourceName(resource_name), weight(weight_) {}

AutomationLink::AutomationLink(Game &game_, const std::string &str): game(&game_) {
	const std::vector<std::string> pieces = split(str, ":", false);
	producer = game->processorsByID.at(pieces[0]);
	consumer = game->processorsByID.at(pieces[1]);
	resourceName = pieces[2];
	weight = parseDouble(pieces[3]);
}

std::string AutomationLink::toString() const {
	return producer->id + ":" + consumer->id + ":" + resourceName + ":" + std::to_string(weight);
}

void AutomationLink::tick() {
	if (producer->output.count(resourceName) == 0)
		return;
	consumer->input[resourceName] += producer->output.at(resourceName);
	producer->output.erase(resourceName);
}
