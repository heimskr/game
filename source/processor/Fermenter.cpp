#include <list>

#include "Game.h"
#include "imgui.h"
#include "main.h"
#include "Resource.h"
#include "processor/Fermenter.h"

Fermenter::Fermenter(Game &game_, double yeast_, const std::map<std::string, double> &input_, const std::map<std::string, double> &output_, bool auto_extract):
	Processor(game_, input_, output_, auto_extract), yeast(yeast_) {}

Fermenter::Fermenter(Game &game_): Processor(game_) {}

std::string Fermenter::toString() const {
	return Processor::toString() + ":" + std::to_string(yeast);
}

double Fermenter::tick(double delta) {
	if (input.count("Yeast") != 0) {
		double &amount = input.at("Yeast");
		const double to_remove = std::min(delta, amount);
		amount -= to_remove;
		yeast += to_remove;
	}

	double out = 0.;

	if (yeast < Resource::MIN_AMOUNT) {
		yeast = 0.;
	} else {
		for (auto &[name, amount]: input) {
			const Resource &resource = game->resources.at(name);
			if (resource.conversions.count(getType()) == 0)
				continue;
			const auto &conversion = resource.conversions.at(getType());
			const double to_convert = std::min(yeast, std::min(amount, conversion.rate * delta));
			out += to_convert;
			yeast -= to_convert;
			amount -= to_convert;
			output[conversion.outName] += to_convert * conversion.amount;
			if (yeast < Resource::MIN_AMOUNT)
				break;
		}
		moveOutput();
	}

	shrink(input);
	return out;
}

void Fermenter::headerAdditional(Context &, long) {
	ImGui::SameLine();
	ImGui::Text("(yeast: %.2f)", yeast);
}

void Fermenter::headerButtons(Context &context, long index) {
	// TODO: allow player to choose a divisor (e.g., 4 to move only a fourth of all fermentables) by holding left on the
	// d-pad when clicking the button
	if (ImGui::Button(("F##fill_" + std::to_string(index)).c_str(), {34.f, 0.f})) {
		for (auto &[name, amount]: context->inventory)
			if (context->resources.at(name).hasType("fermenter fill")) {
				input[name] += amount;
				amount = 0;
			}
		shrink(input);
	}
	if (ImGui::IsItemHovered())
		ImGui::SetTooltip("Fill with fermentable items.");
}