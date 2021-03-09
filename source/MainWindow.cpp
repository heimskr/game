#include "Game.h"
#include "imgui.h"
#include "main.h"
#include "MainWindow.h"

void MainWindow(Context &context, bool *open) {
	ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(1280.0f, 720.0f), ImGuiCond_Once);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);

	if (ImGui::Begin("Trade Game", open, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse)) {
	// if (ImGui::Begin("Trade Game", open, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse)) {
		if (ImGui::BeginMenuBar()) {
			if (ImGui::BeginMenu("Menu")) {
				if (ImGui::MenuItem("Load")) context.showLoadWindow = true;
				if (ImGui::MenuItem("Save")) context.save();
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}
	}

	ImGui::End();
	ImGui::PopStyleVar();
}
