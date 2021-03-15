#include "Game.h"
#include "imgui.h"
#include "Keyboard.h"
#include "main.h"
#include "MainWindow.h"

void MainWindow::renderCrafting() {
	if (!context.game || !context.loaded) {
		ImGui::Text("There's nothing to craft in the void.");
		return;
	}

	if (!context->craftingOutputReady) {
		computeCraftingOutput();
		context->craftingOutputReady = true;
	}

	if (ImGui::Button("Add Item"))
		context.pickInventory([this](const std::string &name) {
			if (context->inventory.count(name) == 0)
				return;
			Keyboard::openForDouble([this, &name](double choice) {
				if (choice == 0)
					choice = context->inventory[name];
				if (choice <= 0 || ltna(context->inventory[name], choice)) {
					context.showMessage("Invalid amount.");
					return;
				}
				context->inventory[name] -= choice;
				context->craftingInventory[name] += choice;
				shrink(context->inventory);
				computeCraftingOutput();
			});
		});

	if (ImGui::BeginTable("Crafting Interface", 4)) {
		const float padding = ImGui::GetStyle().FramePadding.x * 2.f;
		ImGui::TableSetupColumn("Input", ImGuiTableColumnFlags_WidthStretch);
		ImGui::TableSetupColumn("Amount##input", ImGuiTableColumnFlags_WidthFixed, 240.f - padding);
		ImGui::TableSetupColumn("Output", ImGuiTableColumnFlags_WidthStretch);
		ImGui::TableSetupColumn("Amount##output", ImGuiTableColumnFlags_WidthFixed, 240.f - padding);
		ImGui::TableHeadersRow();
		const u64 inv_size = context->craftingInventory.size();
		const u64 out_size = craftingOutput.size();
		const u64 max = std::max(inv_size, out_size);
		auto inv_iter = context->craftingInventory.begin();
		auto out_iter = craftingOutput.begin();
		for (u64 i = 0; i < max; ++i) {
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			if (i < inv_size) {
				if (ImGui::Selectable((inv_iter->first + "##input").c_str()))
					Keyboard::openForDouble([this, inv_iter](double chosen) {
						if (chosen == 0)
							chosen = inv_iter->second;
						if (chosen <= 0 || ltna(inv_iter->second, chosen)) {
							context.showMessage("Invalid amount.");
							return;
						}
						inv_iter->second -= chosen;
						context->inventory[inv_iter->first] += chosen;
						context.frameActions.push_back([this] {
							shrink(context->craftingInventory);
						});
					});
			} else
				ImGui::Dummy({0.f, 0.f});
			ImGui::TableNextColumn();
			ImGui::TableSetColumnIndex(1);
			if (i < inv_size)
				ImGui::Text("%.2f", inv_iter->second);
			else
				ImGui::Dummy({0.f, 0.f});
			ImGui::TableNextColumn();
			ImGui::TableSetColumnIndex(2);
			if (i < out_size) {
				const CraftingRecipe &recipe = **out_iter;
				if (ImGui::Selectable((recipe.output + "##output_" + std::to_string(i)).c_str())) {
					for (const auto &[name, amount]: recipe.inputs)
						context->craftingInventory[name] -= amount;
					context->inventory[recipe.output] += recipe.amount;
					context.frameActions.push_back([this] {
						shrink(context->craftingInventory);
						computeCraftingOutput();
					});
				}
			} else
				ImGui::Dummy({0.f, 0.f});
			ImGui::TableNextColumn();
			ImGui::TableSetColumnIndex(3);
			if (i < out_size)
				ImGui::Text("%.2f", (*out_iter)->amount);
			else
				ImGui::Dummy({0.f, 0.f});
			ImGui::TableNextColumn();
			if (i < inv_size)
				++inv_iter;
			if (i < out_size)
				++out_iter;
		}
		ImGui::EndTable();
	}
}

void MainWindow::computeCraftingOutput() {
	craftingOutput.clear();
	for (const CraftingRecipe &recipe: context->recipes.crafting)
		if (contains(context->craftingInventory, recipe.inputs))
			craftingOutput.push_back(&recipe);
}
