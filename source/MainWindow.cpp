#include "Game.h"
#include "Region.h"
#include "imgui.h"
#include "main.h"
#include "MainWindow.h"

void MainWindow(Context &context, bool *open) {
	ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(1280.0f, 720.0f), ImGuiCond_Once);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);

	if (ImGui::Begin("Trade Game", open, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse)) {
		if (ImGui::BeginMenuBar()) {
			if (ImGui::BeginMenu("Menu")) {
				if (ImGui::MenuItem("Load"))
					context.load();
				if (context.loaded && ImGui::MenuItem("Save"))
					context.save();
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}
	}

	if (!context.game) {
		ImGui::Text("No game is loaded.");
	} else {
		for (const auto &[pos, region]: context.game->regions) {
			const std::string label = region.name + " (" + std::to_string(pos.first) + ", " + std::to_string(pos.second) + ")";
			if (ImGui::CollapsingHeader(label.c_str(), ImGuiTreeNodeFlags_None)) {
				if (region.areas.empty()) {
					ImGui::Text("Region has no areas.");
				} else {
					for (const auto &[name, area]: region.areas) {
						ImGui::Text(name.c_str());
					}
				}
			}
		}
	}

	ImGui::End();
	ImGui::PopStyleVar();
}
