#include "Game.h"
#include "Region.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "main.h"
#include "MainWindow.h"
#include "Keyboard.h"
#include "UI.h"
#include "Direction.h"

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

	Region *region = nullptr;
	try {
		region = &context->currentRegion();
	} catch (const std::out_of_range &) {}

	if (ImGui::BeginTabBar("##tabs", ImGuiTabBarFlags_None)) {
		if (ImGui::BeginTabItem("Region")) {
			if (!context.game || !context.loaded) {
				ImGui::Text("No game is loaded.");
			} else if (region) {
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

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Travel")) {
			if (!region) {
				ImGui::Text("Travel is not possible within the void.");
			} else {
				ImGui::Columns(2, nullptr, false);

				static int set_width = 0;
				if (set_width == 0) {
					ImGui::SetColumnWidth(-1, 110.f);
					++set_width;
				}

				for (int i = 0; i < 4; ++i) {
					Direction direction = static_cast<Direction>(i);
					const bool disabled = !region->hasNeighbor(direction);
					if (disabled) {
						ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
						ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
					}
					if (ImGui::Button(toString(direction), ImVec2(100.f, 0.f))) {
						const auto &offset = getOffset(direction);
						context->position = {context->position.first + offset.first, context->position.second + offset.second};
					}
					if (disabled) {
						ImGui::PopItemFlag();
						ImGui::PopStyleVar();
					}
					ImGui::NextColumn();
					if (Region *neighbor = region->getNeighbor(direction))
						ImGui::Text("%s", neighbor->name.c_str());
					ImGui::NextColumn();
				}

				ImGui::Columns(1);
			}
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	ImGui::End();
	ImGui::PopStyleVar();
}
