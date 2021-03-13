#include "imgui.h"
#include "imgui_internal.h"
#include "Game.h"
#include "main.h"
#include "MainWindow.h"

void MainWindow::renderTravel(Region *region) {
	if (!region) {
		ImGui::Text("Travel is not possible within the void.");
	} else {
		ImGui::Text("Your position is (%ld, %ld).", region->position.first, region->position.second);
		if (ImGui::BeginTable("Map", 3)) {
			constexpr float COLUMN_WIDTH = 200.f;
			ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, COLUMN_WIDTH);
			ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, COLUMN_WIDTH);
			ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, COLUMN_WIDTH);
			for (int row = 0; row < 3; ++row) {
				ImGui::TableNextRow();
				for (int col = 0; col < 3; ++col) {
					ImGui::TableSetColumnIndex(col);
					const Region::Position pos(region->position.first + col - 1, region->position.second + row - 1);
					std::string label = "~";
					bool exists = false;
					if (context->regions.count(pos) != 0) {
						label = context->regions.at(pos)->name;
						exists = true;
					}
					label += "##" + std::to_string(col) + "_" + std::to_string(row);
					const bool disabled = row == 1 && col == 1;
					if (disabled) {
						ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
						ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.75f);
					}
					if (ImGui::Button(label.c_str(), {COLUMN_WIDTH, 100.f})) {
						if (exists)
							context->position = pos;
						else
							*context.game += Region::generate(*context.game, pos);
					}
					if (disabled) {
						ImGui::PopItemFlag();
						ImGui::PopStyleVar();
					}
					ImGui::TableNextColumn();
				}
			}
			ImGui::EndTable();
		}
	}
}
