#include "Processor.h"
#include "Resource.h"
#include "Util.h"
#include "processor/Processors.h"
#include "Game.h"
#include "Main.h"
#include "Keyboard.h"
#include "imgui.h"

Processor::Processor(Game &owner_, const Resource::Map &input_, const Resource::Map &output_, bool auto_extract):
	owner(&owner_), input(input_), output(output_), autoExtract(auto_extract) {}

Processor::Processor(Game &owner_): Processor(owner_, {}, {}, false) {}

std::string Processor::toString() const {
	return std::to_string(static_cast<int>(getType())) + ":" + stringify(input) + ":" + stringify(output) + ":"
		+ (autoExtract? "1" : "0");
}

double Processor::tick(double delta) {
	double out = 0.;

	for (auto &[name, amount]: input) {
		const Resource &resource = owner->resources.at(name);
		if (resource.conversions.count(getType()) != 0) {
			const auto &conversion = resource.conversions.at(getType());
			const double to_convert = std::min(amount, conversion.rate * delta);
			out += to_convert;
			amount -= to_convert;
			output[conversion.outName] += to_convert * conversion.amount;
		}
	}

	shrink(input);
	return out;
}

void Processor::renderHeader(Context &context, long index) {
	ImGui::Dummy({0.f, 20.f});
	if (ImGui::Button(("+##" + std::to_string(index)).c_str()))
		context.pickInventory([this, &context](const std::string &name) {
			if (context->inventory.count(name) == 0) {
				context.showMessage("You don't have any " + name + ".");
				return;
			}
			Keyboard::openForDouble([this, &context, &name](double chosen) {
				if (chosen <= 0) {
					context.showMessage("Invalid amount.");
				} else if (ltna(context->inventory[name], chosen)) {
					context.showMessage("You don't have enough " + name + ".");
				} else {
					context->inventory[name] -= chosen;
					shrink(context->inventory, name);
					input[name] += chosen;
				}
			}, "Resource Amount");
		});
	if (ImGui::IsItemHovered())
		ImGui::SetTooltip("Add resource to processor");
	ImGui::SameLine();
	ImGui::Text("%s", Processor::typeName(getType()));
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
						} else if (ltna(amount, chosen)) {
							context.showMessage("There isn't enough of that resource.");
						} else {
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

Processor * Processor::fromString(Game &owner, const std::string &str) {
	const std::vector<std::string> pieces = split(str, ":", false);
	Resource::Map input(parseMap(pieces[1])), output(parseMap(pieces[2]));
	const bool auto_extract = parseLong(pieces[3]) != 0;
	const Type type = static_cast<Type>(parseLong(pieces[0]));
	switch (type) {
		case Type::Furnace:    return new Furnace(owner, parseDouble(pieces[4]), std::move(input), std::move(output), auto_extract);
		case Type::Centrifuge: return new Centrifuge(owner, std::move(input), std::move(output), auto_extract);
		case Type::Fermenter:  return new Fermenter(owner, parseDouble(pieces[4]), std::move(input), std::move(output), auto_extract);
		default: throw std::invalid_argument("Invalid Processor type: " + std::to_string(static_cast<int>(type)));
	}
}

Processor * Processor::ofType(Game &owner, Type type) {
	switch (type) {
		case Type::Furnace:    return new Furnace(owner);
		case Type::Centrifuge: return new Centrifuge(owner);
		case Type::Fermenter:  return new Fermenter(owner);
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
