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
							context->automationLinks.emplace_back(*context.game, source, destination, resource_name,
								weight);
							context.showMessage("Linked " + source->name + " to " + destination->name + " for "
								 + resource_name + ".");
						}
					}, "Weight", "", 64, "1");
				}, "Select destination processor:");
			});
		}, "Select source processor:");
	}

	if (context->automationLinks.empty()) {
		ImGui::Text("No automation links have been set up.");
		return;
	}

	if (ImGui::BeginTable("Automations", 5)) {
		ImGui::TableSetupColumn("##remove", ImGuiTableColumnFlags_WidthFixed, 40.f);
		ImGui::TableSetupColumn("Source", ImGuiTableColumnFlags_WidthStretch);
		ImGui::TableSetupColumn("Destination", ImGuiTableColumnFlags_WidthStretch);
		ImGui::TableSetupColumn("Resource", ImGuiTableColumnFlags_WidthAlwaysAutoResize);
		ImGui::TableSetupColumn("Weight", ImGuiTableColumnFlags_WidthAlwaysAutoResize);
		ImGui::TableHeadersRow();
		u64 i = 0;
		for (const AutomationLink &link: context->automationLinks) {
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			if (ImGui::Button(("-##remove_" + std::to_string(i)).c_str(), {34.f, 0.f})) {
				context.frameActions.push_back([this, i]() {
					context->automationLinks.erase(std::next(context->automationLinks.begin(), i));
				});
			}
			ImGui::TableNextColumn();
			ImGui::TableSetColumnIndex(1);
			ImGui::Text("%s", link.producer->name.c_str());
			ImGui::TableNextColumn();
			ImGui::TableSetColumnIndex(2);
			ImGui::Text("%s", link.consumer->name.c_str());
			ImGui::TableNextColumn();
			ImGui::TableSetColumnIndex(3);
			ImGui::Text("%s", link.resourceName.c_str());
			ImGui::TableNextColumn();
			ImGui::TableSetColumnIndex(4);
			ImGui::Text("%.2f", link.weight);
			ImGui::TableNextColumn();
			++i;
		}
		ImGui::EndTable();
	}
}
