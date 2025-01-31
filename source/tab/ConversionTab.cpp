#include "Game.h"
#include "imgui.h"
#include "Keyboard.h"
#include "main.h"
#include "MainWindow.h"

void MainWindow::renderConversion() {
	if (!context.game || !context.loaded) {
		ImGui::Text("You can't convert anything in the void.");
	} else  {
		if (ImGui::Button("Add Processor"))
			context.pickProcessorType([this](Processor::Type type) {
				if (context->processorCosts.count(type) == 0) {
					context.showMessage("There is no recipe for " + std::string(Processor::typeName(type))
						+ " processors.");
					return;
				}
				const Resource::Map &cost = context->processorCosts.at(type);
				if (!contains(context->inventory, cost)) {
					context.showMessage("You don't have enough resources to make that.");
					return;
				}
				for (const auto &[name, amount]: cost)
					context->inventory[name] -= amount;
				shrink(context->inventory);
				std::shared_ptr<Processor> new_processor;
				try {
					new_processor = std::shared_ptr<Processor>(Processor::ofType(*context.game, type));
				} catch (const std::invalid_argument &err) {
					context.showMessage("Error: " + std::string(err.what()));
					return;
				}
				new_processor->name = Processor::typeName(type);
				context->processors.push_back(new_processor);
				context->processorsByID.emplace(new_processor->id, new_processor);
				context.showMessage("Added a new " + std::string(Processor::typeName(type)) + ".");
			});
		ImGui::SameLine();
		if (ImGui::Button("Sort"))
			context->processors.sort([](std::shared_ptr<Processor> &left, std::shared_ptr<Processor> &right) {
				return left->name < right->name;
			});
		ImGui::SameLine();
		if (ImGui::Button("Distribute"))
			context.pickInventory([this](const std::string &name) {
				double amount = context->inventory[name];
				if (context.rightDown)
					Keyboard::openForDouble(context, [this, &amount](double chosen) {
						if (chosen <= 0 || ltna(amount, chosen))
							context.showMessage("Invalid amount.");
						else
							amount = chosen;
					}, "Amount to Distribute");
				context.pickProcessorType([this, &name](Processor::Type type) {
					u64 count = 0;
					for (const auto &processor: context->processors)
						if (processor->getType() == type)
							++count;
					if (count == 0) {
						context.showMessage("You don't have any " + std::string(Processor::typeName(type)) + " processors.");
					} else {
						double &amount = context->inventory[name];
						for (auto &processor: context->processors)
							if (processor->getType() == type)
								processor->input[name] += amount / count;
						context->inventory[name] -= amount;
						shrink(context->inventory);
					}
				});
			});
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("Distribute a given resource evenly among all processors of a given type.");
		if (context->processors.empty()) {
			ImGui::Text("You have no processors.");
		} else {
			u64 i = 0;
			for (const auto &processor: context->processors) {
				processor->renderHeader(context, ++i);
				processor->renderBody(context, ++i);
			}
		}
	}
}
