#include "Game.h"
#include "imgui.h"
#include "Keyboard.h"
#include "main.h"
#include "MainWindow.h"

void MainWindow::renderAutomation() {
	if (!context.game || !context.loaded) {
		ImGui::Text("There's nothing to automate in the void.");
		return;
	}

	if (ImGui::Button("+", {34.f, 0.f})) {
		context.pickProcessor([this](std::shared_ptr<Processor> source) {
			context.pickResource([this, source](const std::string &resource_name) {
				context.pickProcessor([this, source, resource_name](std::shared_ptr<Processor> destination) {
					Keyboard::openForDouble([this, &source, &resource_name, &destination](double weight) {
						if (weight <= 0) {
							context.showMessage("Invalid weight.");
						} else {
							context->automationLinks.emplace_back(*context.game, source.get(), destination.get(),
								resource_name, weight);
							context.showMessage("Linked " + source->name + " to " + destination->name + " for "
								 + resource_name + ".");
						}
					}, "Weight", "", 64, "1");
				}, "Select destination processor:");
			});
		}, "Select source processor:");
	}
}
