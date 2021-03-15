#include "Processor.h"
#include "Resource.h"
#include "Util.h"
#include "processor/Processors.h"
#include "Game.h"
#include "main.h"
#include "Keyboard.h"
#include "imgui.h"

Processor::Processor(Game &game_): game(&game_) {
	Logger::info("%s:%d", __FILE__, __LINE__);
}

Processor & Processor::setInput(const std::map<std::string, double> &input_) {
	input = input_;
	return *this;
}

Processor & Processor::setOutput(const std::map<std::string, double> &output_) {
	output = output_;
	return *this;
}

Processor & Processor::setInput(std::map<std::string, double> &&input_) {
	input = std::move(input_);
	return *this;
}

Processor & Processor::setOutput(std::map<std::string, double> &&output_) {
	output = std::move(output_);
	return *this;
}

Processor & Processor::setAutoExtract(bool auto_extract) {
	autoExtract = auto_extract;
	return *this;
}

Processor & Processor::setName(const std::string &name_) {
	name = name_;
	return *this;
}

Processor & Processor::setName(std::string &&name_) {
	name = std::move(name_);
	return *this;
}

Processor & Processor::setID(const std::string &id_) {
	id = id_;
	return *this;
}

Processor & Processor::setID(std::string &&id_) {
	id = std::move(id_);
	return *this;
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
		std::list<const std::string *> to_remove;
		for (auto &[name, amount]: output)
			if (frozen.count(name) == 0) {
				game->inventory[name] += amount;
				to_remove.push_back(&name);
			}
		for (const std::string *name: to_remove)
			output.erase(*name);
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
	if (ImGui::Button(("R##" + index_str).c_str(), {34.f, 0.f}))
		Keyboard::openForText([this, &context](std::string new_name) {
			if (new_name.find_first_of(INVALID_CHARS) != std::string::npos)
				context.showMessage("Invalid name.");
			else
				name = new_name;
		}, "Processor Name", "", 64, name);
	if (ImGui::IsItemHovered())
		ImGui::SetTooltip("Rename");
	ImGui::SameLine();
	headerButtons(context, index);
	ImGui::SameLine();
	ImGui::Text("%s", name.c_str());
	if (context->cheatsEnabled && ImGui::IsItemHovered())
		ImGui::SetTooltip("ID: %s", id.c_str());
	headerAdditional(context, index);
}

void Processor::renderBody(Context &context, long index) {
	if (ImGui::BeginTable(("Layout##" + std::to_string(index)).c_str(), 2)) {
		const float width = ImGui::GetContentRegionMax().x / 2.f - ImGui::GetStyle().WindowPadding.x;
		ImGui::TableSetupColumn("##input_table", ImGuiTableColumnFlags_WidthFixed, width);
		ImGui::TableSetupColumn("##output_table", ImGuiTableColumnFlags_WidthFixed, width);
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		if (ImGui::BeginTable(("##input_" + std::to_string(index)).c_str(), 2)) {
			ImGui::TableSetupColumn("Input Resource", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableSetupColumn("Amount##input", ImGuiTableColumnFlags_WidthFixed, 300.f);
			ImGui::TableHeadersRow();
			u64 j = 0;
			for (auto &[name, amount]: input) {
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				if (ImGui::Selectable((name + "##insel_" + std::to_string(++j)).c_str()))
					Keyboard::openForDouble([this, &context, &name, &amount](double chosen) {
						if (chosen <= 0) {
							context.showMessage("Invalid amount.");
						} else {
							chosen = std::min(amount, chosen);
							amount -= chosen;
							context->inventory[name] += chosen;
							context.frameActions.push_back([this, &name] {
								shrink(input, name);
							});
						}
					});
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
			if (output.empty()) {
				const float text_size = ImGui::CalcTextSize("Y").y;
				const float padding = 2.f * ImGui::GetStyle().FramePadding.y;
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::Dummy({0.f, text_size + padding});
				ImGui::NextColumn();
				ImGui::TableSetColumnIndex(1);
				ImGui::Dummy({0.f, text_size + padding});
				ImGui::NextColumn();
			} else {
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
			}
			ImGui::EndTable();
		}
		ImGui::TableNextColumn();
		ImGui::EndTable();
	}
}

void Processor::headerAdditional(Context &, long) {}
void Processor::headerButtons(Context &, long) {}

std::string Processor::toString() const {
	return std::to_string(static_cast<int>(getType())) + ":" + stringify(input) + ":" + stringify(output) + ":"
		+ (autoExtract? "1" : "0") + ":" + name + ":" + id;
}

Processor * Processor::fromString(Game &game, const std::string &str) {
	std::vector<std::string> pieces = split(str, ":", false);
	Resource::Map input(parseMap(pieces[1])), output(parseMap(pieces[2]));
	const bool auto_extract = parseLong(pieces[3]) != 0;
	std::string &name = pieces[4];
	std::string &id = pieces[5];
	const std::string &extra = pieces[6];
	const Type type = static_cast<Type>(parseLong(pieces[0]));
	Processor *out = nullptr;
	switch (type) {
		case Type::Furnace:    out = &((new Furnace(game))->setFuel(parseDouble(extra))); break;
		case Type::Centrifuge: out = new Centrifuge(game); break;
		case Type::Fermenter:  out = &((new Fermenter(game))->setYeast(parseDouble(extra))); break;
		case Type::Crusher:    out = new Crusher(game); break;
		case Type::Refinery:   out = &((new Refinery(game))->setMode(static_cast<RefineryMode>(parseLong(extra)))); break;
		default: throw std::invalid_argument("Invalid Processor type: " + std::to_string(static_cast<int>(type)));
	}

	out->setInput(std::move(input)).setOutput(std::move(output)).setAutoExtract(auto_extract).setName(std::move(name));
	out->setID(std::move(id));
	return out;
}

Processor * Processor::ofType(Game &game, Type type) {
	Logger::info("%s:%d", __FILE__, __LINE__);
	Processor *out;
	Logger::info("%s:%d", __FILE__, __LINE__);
	switch (type) {
		case Type::Furnace:    out = new Furnace(game);    break;
		case Type::Centrifuge: out = new Centrifuge(game); break;
		case Type::Fermenter:  out = new Fermenter(game);  break;
		case Type::Crusher:    out = new Crusher(game);    break;
		case Type::Refinery:   out = new Refinery(game);   break;
		default: throw std::invalid_argument("Invalid Processor type: " + std::to_string(static_cast<int>(type)));
	}
	Logger::info("%s:%d", __FILE__, __LINE__);
	out->setID(std::move(makeUUID()));
	Logger::info("%s:%d", __FILE__, __LINE__);
	return out;
}

const char * Processor::typeName(Type type) {
	switch (type) {
		case Type::Furnace:    return "Furnace";
		case Type::Centrifuge: return "Centrifuge";
		case Type::Fermenter:  return "Fermenter";
		case Type::Crusher:    return "Crusher";
		case Type::Refinery:   return "Refinery";
		default: return "?";
	}
}
