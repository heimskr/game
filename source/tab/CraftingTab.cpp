#include "Game.h"
#include "imgui.h"
#include "main.h"
#include "MainWindow.h"

void MainWindow::renderCrafting() {
	if (!context.game || !context.loaded) {
		ImGui::Text("There's nothing to craft in the void.");
		return;
	}

	if (ImGui::Button("Add Item")) {

	}

	if (ImGui::BeginTable("Crafting Interface", 4)) {
		const float padding = ImGui::GetStyle().FramePadding.x * 2.f;
		ImGui::TableSetupColumn("Input", ImGuiTableColumnFlags_WidthStretch);
		ImGui::TableSetupColumn("Amount##input", ImGuiTableColumnFlags_WidthFixed, 240.f - padding);
		ImGui::TableSetupColumn("Output", ImGuiTableColumnFlags_WidthStretch);
		ImGui::TableSetupColumn("Amount##output", ImGuiTableColumnFlags_WidthFixed, 240.f - padding);
		ImGui::TableHeadersRow();
		if (craftingOutput.empty()) {
			craftingOutput.emplace("Microchip", 10.);
			craftingOutput.emplace("Plastic", 10.);
		}
		const u64 inv_size = context->craftingInventory.size();
		const u64 out_size = craftingOutput.size();
		const u64 max = std::max(inv_size, out_size);
		auto inv_iter = context->craftingInventory.begin();
		auto out_iter = craftingOutput.begin();
		for (u64 i = 0; i < max; ++i) {
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			if (i < inv_size) {
				if (ImGui::Selectable((inv_iter->first + "##input").c_str())) {
					context.showMessage("Remove " + inv_iter->first + " from input");
				}
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
				if (ImGui::Selectable((out_iter->first + "##output").c_str())) {
					context.showMessage("Remove " + out_iter->first + " from output");
				}
			} else
				ImGui::Dummy({0.f, 0.f});
			ImGui::TableNextColumn();
			ImGui::TableSetColumnIndex(3);
			if (i < out_size)
				ImGui::Text("%.2f", out_iter->second);
			else
				ImGui::Dummy({0.f, 0.f});
			ImGui::TableNextColumn();
			++inv_iter;
			++out_iter;
		}
		ImGui::EndTable();
	}
}
