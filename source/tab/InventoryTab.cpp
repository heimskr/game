#include "Game.h"
#include "imgui.h"
#include "Keyboard.h"
#include "main.h"
#include "MainWindow.h"

void MainWindow::renderInventory() {
	if (!context.game || !context.loaded) {
		ImGui::Text("No game is loaded.");
	} else {
		if (context->cheatsEnabled) {
			if (ImGui::Button("+##add_resource", {40.f, 0.f}))
				context.pickResource([this](const std::string &name) {
					Keyboard::openForDouble([this, &name](double chosen) {
						if (chosen <= 0.)
							context.showMessage("Invalid amount.");
						else
							context->inventory[name] += chosen;
					});
				});
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip("Add a resource into your inventory (cheat)");
		}
		if (context->inventory.empty()) {
			ImGui::Text("You are bereft of resources.");
		} else if (ImGui::BeginTable("Inventory Table", 3)) {
			ImGui::TableSetupColumn("Delete", ImGuiTableColumnFlags_WidthFixed, 50.f);
			ImGui::TableSetupColumn("Resource", ImGuiTableColumnFlags_WidthFixed, 240.f);
			ImGui::TableSetupColumn("Amount", ImGuiTableColumnFlags_WidthAlwaysAutoResize);
			std::vector<const std::string *> to_erase;
			to_erase.reserve(context->inventory.size());
			for (const auto &[name, amount]: context->inventory) {
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				if (ImGui::Button(("-##" + name).c_str(), {40.f, 0.f}))
					Keyboard::openForDouble([this, &name, &to_erase](double chosen) {
						double &amount = context->inventory[name];
						if (chosen <= 0)
							context.showMessage("Error: Invalid amount.");
						else if (lte(amount, chosen))
							to_erase.push_back(&name);
						else
							amount -= chosen;
					}, "Amount to Discard");
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip("Discard resource");
				ImGui::TableNextColumn();
				ImGui::TableSetColumnIndex(1);
				ImGui::Text("%s", name.c_str());
				ImGui::TableNextColumn();
				ImGui::TableSetColumnIndex(2);
				ImGui::Text("%.2f", amount);
				ImGui::TableNextColumn();
			}
			for (const std::string *name: to_erase)
				context->inventory.erase(*name);
			ImGui::EndTable();
		}
	}
}
