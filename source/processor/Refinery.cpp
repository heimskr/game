#include "Game.h"
#include "imgui.h"
#include "main.h"
#include "Resource.h"
#include "processor/Refinery.h"

std::array<RefineryMode, 3> refineryModes = {RefineryMode::Clarify, RefineryMode::Congeal, RefineryMode::Polymerize};

Refinery & Refinery::setMode(RefineryMode mode_) {
	mode = mode_;
	return *this;
}

std::string Refinery::toString() const {
	return Processor::toString() + ":" + std::to_string(static_cast<int>(mode));
}

double Refinery::tick(double delta) {
	double out = 0.;

	const auto &recipes = game->recipes.refinery;

	for (const RefineryRecipe &recipe: recipes) {
		if (recipe.mode != mode)
			continue;
		bool enough = true;
		for (const auto &[name, amount]: recipe.inputs) {
			if (input.count(name) == 0 || ltna(input.at(name) * delta, amount * delta)) {
				enough = false;
				break;
			}
		}
		if (!enough)
			continue;
		for (const auto &[name, amount]: recipe.inputs)
			input[name] -= amount * delta;
		for (const auto &[name, amount]: recipe.outputs)
			output[name] += amount * delta;
	}

	shrink(input);
	moveOutput();
	return out;
}

void Refinery::headerAdditional(Context &context, long index) {
	ImGui::SameLine();
	ImGui::Text("(%s)", stringify(mode));
}

void Refinery::headerButtons(Context &context, long index) {
	if (ImGui::Button(("M##mode_" + std::to_string(index)).c_str(), {34.f, 0.f}))
		context.pickRefineryMode([this, &context](RefineryMode mode_) {
			mode = mode_;
		});
	if (ImGui::IsItemHovered())
		ImGui::SetTooltip("Set refinery mode.");
}

const char * stringify(RefineryMode mode) {
	switch (mode) {
		case RefineryMode::Clarify:    return "Clarify";
		case RefineryMode::Congeal:    return "Congeal";
		case RefineryMode::Polymerize: return "Polymerize";
		default: return "???";
	}
}
