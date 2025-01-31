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
			constexpr float COLUMN_WIDTH = 250.f, COLUMN_HEIGHT = 100.f;
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
					bool populated = false;
					if (context->regions.count(pos) != 0) {
						const Region &neighbor = *context->regions.at(pos);
						label = neighbor.name;
						populated = neighbor.occupied();
						exists = true;
					}
					const std::string bare_label = label;
					label += "##" + std::to_string(col) + "_" + std::to_string(row);
					const bool disabled = row == 1 && col == 1;
					if (disabled) {
						ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
						ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.75f);
					}
					const bool is_center = pos.first == 0 && pos.second == 0;
					if (is_center) {
						ImGui::PushStyleColor(ImGuiCol_Button, {0.f, 0.4f, 0.f, 1.f});
						ImGui::PushStyleColor(ImGuiCol_ButtonHovered, {0.f, 0.6f, 0.f, 1.f});
						ImGui::PushStyleColor(ImGuiCol_ButtonActive, {0.f, 0.5f, 0.f, 1.f});
					}
					if (ImGui::Button(label.c_str(), {COLUMN_WIDTH, COLUMN_HEIGHT})) {
						context.frameActions.push_back([this, pos, exists] {
							if (exists)
								context->position = pos;
							else
								*context.game += Region::generate(*context.game, pos);
						});
					}
					const ImVec2 button_start = ImGui::GetItemRectMin();
					if (is_center) {
						ImGui::PopStyleColor();
						ImGui::PopStyleColor();
						ImGui::PopStyleColor();
					}
					if (disabled) {
						ImGui::PopItemFlag();
						ImGui::PopStyleVar();
					}
					if (populated) {
						const ImVec2 text_size = ImGui::CalcTextSize(bare_label.c_str());
						const ImVec2 min {
							button_start.x + (COLUMN_WIDTH  - text_size.x) / 2.f,
							button_start.y + (COLUMN_HEIGHT + text_size.y) / 2.f
						};
						const ImVec2 max = {min.x + text_size.x, min.y};
						ImGui::GetWindowDrawList()->AddLine(min, max, 0xffffffff);
					}
					ImGui::TableNextColumn();
				}
			}
			ImGui::EndTable();
		}
	}
}
