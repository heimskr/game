#include "Game.h"
#include "Region.h"
#include "imgui.h"
#include "main.h"
#include "MainWindow.h"
#include "Keyboard.h"
#include "UI.h"

void MainWindow(Context &context, bool *open) {
	ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(1280.0f, 720.0f), ImGuiCond_Once);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);

	if (ImGui::Begin("Trade Game", open, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse)) {
		if (ImGui::BeginMenuBar()) {
			if (ImGui::BeginMenu("Menu")) {
				if (ImGui::MenuItem("Load"))
					context.load();
				if (context.loaded && ImGui::MenuItem("Stringify")) {
					if (context.game)
						context.message = context.game->toString();
					else
						context.message = "Game is null";
				}
				if (context.loaded && ImGui::MenuItem("Save"))
					context.save();
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}
	}

	if (!context.game || !context.loaded) {
		ImGui::Text("No game is loaded.");
	} else {
		Region *region = nullptr;
		try {
			region = &context->currentRegion();
		} catch (const std::exception &err) {
			std::string text = "Error: " + std::string(err.what());
			ImGui::Text("%s", text.c_str());
		}

		if (region) {
			ImGui::PushFont(UI::getFont("Nintendo Standard Big"));
			ImGui::Text("%s", region->name.c_str());
			ImGui::PopFont();
			if (ImGui::Button("Rename")) {
				Keyboard::openForText([&](std::string new_name) {
					if (new_name.empty() || new_name == region->name) {
						context.message = "Name not updated.";
					} else {
						context.message = "Renamed " + region->name + " to " + new_name + ".";
						context->updateName(*region, new_name);
					}
				}, "New Region Name", "", 64, region->name);
			}
			if (region->areas.empty()) {
				ImGui::Dummy(ImVec2(20.f, 0.f));
				ImGui::SameLine();
				ImGui::Text("Region has no areas.");
			} else {
				for (const auto &[name, area]: region->areas) {
					ImGui::SetNextItemOpen(true, ImGuiCond_Once);
					if (ImGui::TreeNode(name.c_str())) {
						for (const auto &[rname, amount]: area->resources) {
							ImGui::Dummy(ImVec2(20.f, 0.f));
							ImGui::SameLine();
							ImGui::PushID(rname.c_str());
							if (ImGui::Button("Extract")) {
								Keyboard::openForDouble([&](double chosen) {
									if (amount < chosen) {
										context.message = "Not enough of that resource is available.";
									} else {
										context->extractions.emplace_back(area.get(), rname, chosen, context->resources.at(rname).defaultExtractionRate);
										context.message = "Extracting " + std::to_string(chosen) + " x " + rname + ".";
									}
								}, "Amount to Extract");
							}
							ImGui::PopID();
							ImGui::SameLine();
							ImGui::Text("%s x %.2f", rname.c_str(), amount);
						}
						ImGui::TreePop();
					}
				}
			}
		}
	}

	ImGui::End();
	ImGui::PopStyleVar();
}
