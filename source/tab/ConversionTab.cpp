#include "Game.h"
#include "imgui.h"
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
				context->processors.push_back(std::unique_ptr<Processor>(Processor::ofType(*context.game, type)));
				context.showMessage("Added a new " + std::string(Processor::typeName(type)) + ".");
			});

		if (context->processors.empty()) {
			ImGui::Text("You have no processors.");
		} else {
			u64 i = 0;
			for (const std::unique_ptr<Processor> &processor: context->processors) {
				processor->renderHeader(context, ++i);
				processor->renderBody(context, ++i);
			}
		}
	}
}
