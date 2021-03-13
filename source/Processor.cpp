#include "Processor.h"
#include "Resource.h"
#include "Util.h"
#include "processor/Processors.h"
#include "Game.h"
#include "Main.h"
#include "Keyboard.h"
#include "imgui.h"

Processor::Processor(Game &game_, const Resource::Map &input_, const Resource::Map &output_, bool auto_extract):
	game(&game_), input(input_), output(output_), autoExtract(auto_extract) {}

Processor::Processor(Game &game_): Processor(game_, {}, {}, false) {}

std::string Processor::toString() const {
	return std::to_string(static_cast<int>(getType())) + ":" + stringify(input) + ":" + stringify(output) + ":"
		+ (autoExtract? "1" : "0");
}

double Processor::tick(double delta) {
	double out = 0.;

	for (auto &[name, amount]: input) {
		const Resource &resource = game->resources.at(name);
		if (resource.conversions.count(getType()) != 0) {
			const auto &conversion = resource.conversions.at(getType());
			const double to_convert = std::min(amount, conversion.rate * delta);
			out += to_convert;
			amount -= to_convert;
			output[conversion.outName] += to_convert * conversion.amount;
		}
	}

	shrink(input);
	moveOutput();
	return out;
}

void Processor::moveOutput() {
	if (autoExtract) {
		for (auto &[name, amount]: output)
			game->inventory[name] += amount;
		output.clear();
	}
}

void Processor::renderHeader(Context &context, long index) {
	const std::string index_str = std::to_string(index);
	ImGui::Dummy({0.f, 20.f});
	if (ImGui::Button(("+##" + index_str).c_str(), {34.f, 0.f}))
		context.pickInventory([this, &context](const std::string &name) {
			if (context->inventory.count(name) == 0) {
				context.showMessage("You don't have any " + name + ".");
				return;
			}
			Keyboard::openForDouble([this, &context, &name](double chosen) {
				if (chosen <= 0) {
					context.showMessage("Invalid amount.");
				} else {
					chosen = std::min(context->inventory[name], chosen);
					context->inventory[name] -= chosen;
					shrink(context->inventory, name);
					input[name] += chosen;
				}
			}, "Resource Amount");
		});
	if (ImGui::IsItemHovered())
		ImGui::SetTooltip("Add resource to processor");
	ImGui::SameLine();
	ImGui::Checkbox(("##ae_" + index_str).c_str(), &autoExtract);
	if (ImGui::IsItemHovered())
		ImGui::SetTooltip("Auto-Extract");
	ImGui::SameLine();
	ImGui::Text("%s", Processor::typeName(getType()));
	headerAdditional();
}

void Processor::renderBody(Context &context, long index) {
	if (ImGui::BeginTable(("Layout##" + std::to_string(index)).c_str(), 2)) {
		const float width = ImGui::GetContentRegionMax().x / 2.f;
		ImGui::TableSetupColumn("##input_table", ImGuiTableColumnFlags_WidthFixed, width);
		ImGui::TableSetupColumn("##output_table", ImGuiTableColumnFlags_WidthFixed, width);
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		if (ImGui::BeginTable(("##input" + std::to_string(index)).c_str(), 2)) {
			ImGui::TableSetupColumn("Input Resource", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableSetupColumn("Amount##input", ImGuiTableColumnFlags_WidthFixed, 300.f);
			ImGui::TableHeadersRow();
			for (const auto &[name, amount]: input) {
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::Text("%s", name.c_str());
				ImGui::TableNextColumn();
				ImGui::TableSetColumnIndex(1);
				ImGui::Text("%.3f", amount);
				ImGui::TableNextColumn();
			}
			ImGui::EndTable();
		}
		ImGui::TableNextColumn();
		ImGui::TableSetColumnIndex(1);
		if (ImGui::BeginTable(("##output" + std::to_string(index)).c_str(), 2)) {
			ImGui::TableSetupColumn("Output Resource", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableSetupColumn("Amount##output", ImGuiTableColumnFlags_WidthFixed, 300.f);
			ImGui::TableHeadersRow();
			u64 j = 0;
			for (auto &[name, amount]: output) {
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				if (ImGui::Selectable((name + "##outsel_" + std::to_string(++j)).c_str()))
					Keyboard::openForDouble([this, &context, &name, &amount](double chosen) {
						if (chosen <= 0) {
							context.showMessage("Invalid amount.");
						} else {
							chosen = std::min(amount, chosen);
							amount -= chosen;
							context->inventory[name] += chosen;
							context.frameActions.push_back([this, &name] {
								shrink(output, name);
							});
						}
					}, "Amount to Remove");
				ImGui::TableNextColumn();
				ImGui::TableSetColumnIndex(1);
				ImGui::Text("%.3f", amount);
				ImGui::TableNextColumn();
			}
			ImGui::EndTable();
		}
		ImGui::TableNextColumn();
		ImGui::EndTable();
	}
}

void Processor::headerAdditional() {}

Processor * Processor::fromString(Game &game, const std::string &str) {
	const std::vector<std::string> pieces = split(str, ":", false);
	Resource::Map input(parseMap(pieces[1])), output(parseMap(pieces[2]));
	const bool auto_extract = parseLong(pieces[3]) != 0;
	const Type type = static_cast<Type>(parseLong(pieces[0]));
	switch (type) {
		case Type::Furnace:    return new Furnace(game, parseDouble(pieces[4]), std::move(input), std::move(output), auto_extract);
		case Type::Centrifuge: return new Centrifuge(game, std::move(input), std::move(output), auto_extract);
		case Type::Fermenter:  return new Fermenter(game, parseDouble(pieces[4]), std::move(input), std::move(output), auto_extract);
		default: throw std::invalid_argument("Invalid Processor type: " + std::to_string(static_cast<int>(type)));
	}
}

Processor * Processor::ofType(Game &game, Type type) {
	switch (type) {
		case Type::Furnace:    return new Furnace(game);
		case Type::Centrifuge: return new Centrifuge(game);
		case Type::Fermenter:  return new Fermenter(game);
		default: throw std::invalid_argument("Invalid Processor type: " + std::to_string(static_cast<int>(type)));
	}
}

const char * Processor::typeName(Type type) {
	switch (type) {
		case Type::Furnace:    return "Furnace";
		case Type::Centrifuge: return "Centrifuge";
		case Type::Fermenter:  return "Fermenter";
		default: return "?";
	}
}
