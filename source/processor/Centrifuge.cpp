#include <list>

#include "Game.h"
#include "imgui.h"
#include "main.h"
#include "Resource.h"
#include "processor/Centrifuge.h"

double Centrifuge::tick(double delta) {
	double out = 0.;

	const auto &recipes = game->recipes.centrifuge;

	for (auto &[rname, amount]: input)
		if (recipes.count(rname) != 0) {
			const std::pair<std::string, double> choice = recipes.at(rname).choose();
			// TODO: possibly allow centrifuge recipes to consume more than 1 of the resource per second
			const double to_convert = std::min(amount, delta);
			amount -= to_convert;
			if (!choice.first.empty()) {
				output[choice.first] += to_convert * choice.second;
				out += to_convert;
			}
		}

	shrink(input);
	moveOutput();
	return out;
}

void Centrifuge::headerButtons(Context &context, long index) {
	if (ImGui::Button(("F##fill_" + std::to_string(index)).c_str(), {34.f, 0.f})) {
		for (auto &[name, amount]: context->inventory)
			if (context->resources.at(name).hasType("centrifugable")) {
				input[name] += amount;
				amount = 0;
			}
		shrink(input);
	}
	if (ImGui::IsItemHovered())
		ImGui::SetTooltip("Fill with centrifugable items.");
}
