#include "AutomationLink.h"
#include "Processor.h"
#include "Util.h"

AutomationLink::AutomationLink(Game &game_, Processor *producer_, Processor *consumer_,
const std::string &resource_name, double weight_):
	game(&game_), producer(producer_), consumer(consumer_), resourceName(resource_name), weight(weight_) {}

AutomationLink::AutomationLink(Game &game_, const std::string &str): game(&game_) {
	const std::vector<std::string> pieces = split(str, ":", false);

}

std::string AutomationLink::toString() const {
	return producer->id + ":" + consumer->id + ":" + resourceName + ":" + std::to_string(weight);
}

void AutomationLink::tick() {

}
