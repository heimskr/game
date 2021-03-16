#include <list>

#include "Game.h"
#include "imgui.h"
#include "main.h"
#include "Resource.h"
#include "processor/RocketFurnace.h"

RocketFurnace & RocketFurnace::setHydrogen(double hydrogen_) {
	hydrogen = hydrogen_;
	return *this;
}

RocketFurnace & RocketFurnace::setOxygen(double oxygen_) {
	oxygen = oxygen_;
	return *this;
}

std::string RocketFurnace::toString() const {
	return Processor::toString() + ":" + std::to_string(hydrogen) + ":" + std::to_string(oxygen);
}

double RocketFurnace::tick(double delta) {
	if (input.count("Hydrogen") != 0) {
		double &amount = input.at("Hydrogen");
		const double to_remove = std::min(delta * 20., amount);
		amount -= to_remove;
		hydrogen += to_remove;
	}

	if (input.count("Oxygen") != 0) {
		double &amount = input.at("Oxygen");
		const double to_remove = std::min(delta * 10., amount);
		amount -= to_remove;
		oxygen += to_remove;
	}

	double out = 0.;

	if (hydrogen < Resource::MIN_AMOUNT / 2.) {
		hydrogen = 0.;
	} else if (oxygen < Resource::MIN_AMOUNT) {
		oxygen = 0.;
	} else {
		for (auto &[name, amount]: input) {
			const Resource &resource = game->resources.at(name);
			double to_convert = 0;
			if (resource.conversions.count(Type::RocketFurnace) != 0) {
				const auto &conversion = resource.conversions.at(Type::RocketFurnace);
				to_convert = std::min(hydrogen / 2., std::min(oxygen, std::min(amount, conversion.rate * 10. * delta)));
				output[conversion.outName] += to_convert * conversion.amount;
			} else if (resource.conversions.count(Type::Furnace) != 0) {
				const auto &conversion = resource.conversions.at(Type::Furnace);
				to_convert = std::min(hydrogen / 2., std::min(oxygen, std::min(amount, conversion.rate * 10. * delta)));
				output[conversion.outName] += to_convert * conversion.amount;
			} else {
				continue;
			}
			out += to_convert;
			hydrogen -= to_convert * 2.;
			oxygen -= to_convert;
			amount -= to_convert;
			if (hydrogen < Resource::MIN_AMOUNT * 2. || oxygen < Resource::MIN_AMOUNT)
				break;
		}
		moveOutput();
	}

	shrink(input);
	return out;
}

void RocketFurnace::headerAdditional(Context &, long) {
	ImGui::SameLine();
	ImGui::Text("(H: %.2f, O: %.2f)", hydrogen, oxygen);
}

void RocketFurnace::headerButtons(Context &context, long index) {
	if (ImGui::Button(("F##fill_" + std::to_string(index)).c_str(), {34.f, 0.f})) {
		if (context->inventory.count("Hydrogen") != 0 && context->inventory.count("Oxygen") != 0) {
			// Ensure the ratio of fuel added is always 2 Hydrogen per 1 Oxygen.
			double oxygen = context->inventory.at("Oxygen");
			double hydrogen = std::min(context->inventory.at("Hydrogen"), oxygen * 2);
			oxygen = std::min(oxygen, hydrogen / 2.);
			input["Hydrogen"] += hydrogen;
			context->inventory.at("Hydrogen") -= hydrogen;
			input["Oxygen"] += oxygen;
			context->inventory.at("Oxygen") -= oxygen;
			for (auto &[name, amount]: context->inventory) {
				const Resource &resource = context->resources.at(name);
				if (resource.hasType("rocket smeltable")) {
					input[name] += amount;
					amount = 0;
				}
			}
		} else {
			for (auto &[name, amount]: context->inventory) {
				const Resource &resource = context->resources.at(name);
				if (name == "Hydrogen" || name == "Oxygen" || resource.hasType("rocket smeltable")) {
					input[name] += amount;
					amount = 0;
				}
			}
		}
		shrink(input);
	}
	if (ImGui::IsItemHovered())
		ImGui::SetTooltip("Fill with smeltable items, hydrogen and oxygen.");
}
