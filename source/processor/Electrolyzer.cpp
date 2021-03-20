#include <list>

#include "Game.h"
#include "imgui.h"
#include "main.h"
#include "Resource.h"
#include "processor/Electrolyzer.h"

double Electrolyzer::tick(double delta) {
	double out = 0.;

	const auto &recipes = game->recipes.electrolyzer;

	for (auto &[name, amount]: input)
		if (recipes.count(name) != 0) {
			const std::vector<MultiOutput> &outputs = recipes.at(name).outputs;
			// TODO: possibly allow electrolyzer recipes to consume more than 1 of the resource per second
			const double to_convert = std::min(amount, delta);
			amount -= to_convert;
			for (const MultiOutput &result: outputs) {
				output[result.output] += to_convert * result.amount;
				out += to_convert * result.amount;
			}
		}

	shrink(input);
	moveOutput();
	return out;
}

// void Electrolyzer::headerButtons(Context &context, long index) {
// 	if (ImGui::Button(("F##fill_" + std::to_string(index)).c_str(), {34.f, 0.f})) {
// 		for (auto &[name, amount]: context->inventory)
// 			if (context->resources.at(name).hasType("crushable")) {
// 				input[name] += amount;
// 				amount = 0;
// 			}
// 		shrink(input);
// 	}
// 	if (ImGui::IsItemHovered())
// 		ImGui::SetTooltip("Fill with crushable items.");
// }
