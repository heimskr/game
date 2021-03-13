#include "Game.h"
#include "imgui.h"
#include "Keyboard.h"
#include "MainWindow.h"
#include "main.h"
#include "NameGen.h"
#include "Region.h"
#include "UI.h"

void MainWindow::renderRegion(Region *region) {
	if (!context.game || !context.loaded) {
		ImGui::Text("No game is loaded.");
	} else if (region) {
		ImGui::PushFont(UI::getFont("Nintendo Standard Big"));
		ImGui::Text("%s", region->name.c_str());
		ImGui::PopFont();
		ImGui::SameLine();
		ImGui::Dummy({6.f, 0.f});
		ImGui::SameLine();
		if (ImGui::Button("Rename")) {
			Keyboard::openForText([&](std::string new_name) {
				if (new_name.empty() || new_name == region->name) {
					context.showMessage("Name not updated.");
				} else {
					try {
						const std::string old_name = region->name;
						context->updateName(*region, new_name);
						context.showMessage("Renamed " + old_name + " to " + new_name + ".");
					} catch (const std::exception &err) {
						context.showMessage("Error: " + std::string(err.what()));
					}
				}
			}, "New Region Name", "", 64, NameGen::makeRandomLanguage().makeName());
		}

		ImGui::SameLine();
		if (ImGui::Button("Delete")) {
			context.confirm("Do you want to delete " + region->name + "?", [this, region](bool choice) {
				if (choice)
					context.frameActions.push_back([this, region] {
						context->erase(*region);
					});
			});
		}

		ImGui::Dummy({0, 20.f});
		ImGui::Text("Position: (%ld, %ld)", region->position.first, region->position.second);
		ImGui::Text("Size: %lu", region->size);
		ImGui::Dummy({0, 20.f});

		if (region->areas.empty()) {
			ImGui::Dummy(ImVec2(20.f, 0.f));
			ImGui::SameLine();
			ImGui::Text("Region has no areas.");
		} else {
			for (auto &[name, area]: region->areas) {
				ImGui::SetNextItemOpen(true, ImGuiCond_Once);
				if (area->playerOwned)
					ImGui::PushStyleColor(ImGuiCol_Text, {0.f, 1.f, 0.f, 1.f});
				if (ImGui::TreeNode((name + " (" + std::to_string(area->size) + ")").c_str())) {
					if (area->playerOwned)
						ImGui::PopStyleColor();
					ImGui::SameLine(1170.f);
					if (ImGui::Button("M", {40.f, 0.f}))
						context.pickInventory([this, &area](const std::string &name) {
							Keyboard::openForDouble([this, name, &area](double chosen) {
								insert(area, name, chosen);
							}, "Resource Amount");
						});
					if (ImGui::IsItemHovered())
						ImGui::SetTooltip("Move a resource from your inventory into the area.");
					if (area->playerOwned) {
						ImGui::SameLine();
						if (ImGui::Button("R", {40.f, 0.f}))
							Keyboard::openForNumber([this, area](size_t chosen) mutable {
								context.frameActions.push_back([this, area, chosen]() {
									if (chosen < area->size) {
										if (!area->reduceSize(chosen))
											context.showMessage("Couldn't reduce area size.");
									} else if (chosen == area->size)
										context.showMessage("That wouldn't do anything.");
									else
										context.showMessage("Increasing area size is unimplemented.");
								});
							}, "New Size");
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip("Resize the area.");
					}
					for (const auto &[rname, amount]: area->resources) {
						ImGui::Dummy(ImVec2(20.f, 0.f));
						ImGui::SameLine();
						ImGui::PushID(rname.c_str());
						if (ImGui::Button("Extract")) {
							Keyboard::openForDouble([&](double chosen) {
								if (amount < 0)
									context.showMessage("Invalid amount.");
								else if (0 < amount && ltna(amount, chosen))
									context.showMessage("Not enough of that resource is available.");
								else
									context->extract(*area, rname, chosen);
							}, "Amount to Extract");
						}
						ImGui::PopID();
						ImGui::SameLine();
						ImGui::Text("%s x %.2f", rname.c_str(), amount);
						if (Extraction *extraction = context->getExtraction(*area, rname)) {
							ImGui::SameLine();
							ImGui::Dummy({10.f, 0.f});
							ImGui::SameLine();
							ImGui::PushStyleColor(ImGuiCol_Text, {1.f, 0.f, 0.f, 1.f});
							ImGui::Text("- %.2f/s", extraction->rate);
							ImGui::PopStyleColor();
						}
					}
					ImGui::TreePop();
				} else if (area->playerOwned)
					ImGui::PopStyleColor();
			}
		}
	}
}