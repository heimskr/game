#include "Game.h"
#include "Region.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "main.h"
#include "MainWindow.h"
#include "NameGen.h"
#include "Keyboard.h"
#include "UI.h"
#include "Direction.h"

void MainWindow::render(bool *open) {
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
		if (ImGui::BeginTabItem("Region", nullptr, selectedTab == 0? ImGuiTabItemFlags_SetSelected : 0)) {
			lastTab = 0;
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
							context.message = "Name not updated.";
						} else {
							context.message = "Renamed " + region->name + " to " + new_name + ".";
							context->updateName(*region, new_name);
						}
					}, "New Region Name", "", 64, NameGen::makeRandomLanguage().makeName());
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
					for (const auto &[name, area]: region->areas) {
						ImGui::SetNextItemOpen(true, ImGuiCond_Once);
						if (ImGui::TreeNode((name + " (" + std::to_string(area->size) + ")").c_str())) {
							ImGui::SameLine(1200.f);
							if (ImGui::Button("+"))
								context.pickResource([this, &area](const std::string &name) {
									Keyboard::openForDouble([this, name, &area](double chosen) {
										insert(area, name, chosen);
									}, "Resource Amount");
								});
							for (const auto &[rname, amount]: area->resources) {
								ImGui::Dummy(ImVec2(20.f, 0.f));
								ImGui::SameLine();
								ImGui::PushID(rname.c_str());
								if (ImGui::Button("Extract")) {
									Keyboard::openForDouble([&](double chosen) {
										if (amount <= 0)
											context.message = "Invalid amount.";
										else if (amount < chosen)
											context.message = "Not enough of that resource is available.";
										else
											context->extractions.emplace_back(area.get(), rname, chosen, context->resources.at(rname).defaultExtractionRate);
									}, "Amount to Extract");
								}
								ImGui::PopID();
								ImGui::SameLine();
								ImGui::Text("%s x %.2f", rname.c_str(), amount);
								if (Extraction *extraction = context->getExtraction(*area, rname)) {
									ImGui::SameLine();
									ImGui::Dummy({10.f, 0.f});
									ImGui::SameLine();
									ImGui::PushStyleColor(ImGuiCol_Text, {1.0f, 0.f, 0.f, 1.0f});
									ImGui::Text("- %.2f/s", extraction->rate);
									ImGui::PopStyleColor();
								}
							}
							ImGui::TreePop();
						}
					}
				}
			}

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Travel", nullptr, selectedTab == 1? ImGuiTabItemFlags_SetSelected : 0)) {
			lastTab = 1;
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
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Extractions", nullptr, selectedTab == 2? ImGuiTabItemFlags_SetSelected : 0)) {
			lastTab = 2;
			if (context->extractions.empty()) {
				ImGui::Text("Nothing is happening.");
			} else {
				for (const Extraction &extraction: context->extractions) {
					ImGui::Text("Extracting %s from %s in %s (%.2f left) @ %.2f/s",
						extraction.resourceName.c_str(), extraction.area->name.c_str(),
						extraction.area->parent->name.c_str(), extraction.amount, extraction.rate);
					if (0. < extraction.startAmount) {
						static char buf[32];
						snprintf(buf, 32, "%.2f/%.2f", extraction.startAmount - extraction.amount, extraction.startAmount);
						ImGui::ProgressBar((extraction.startAmount - extraction.amount) / extraction.startAmount, {0, 0}, buf);
					}
				}
			}
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Inventory", nullptr, selectedTab == 3? ImGuiTabItemFlags_SetSelected : 0)) {
			lastTab = 3;
			if (!context.game || !context.loaded) {
				ImGui::Text("No game is loaded.");
			} else if (context->inventory.empty()) {
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
								context.message = "Error: Invalid amount.";
							else if (amount <= chosen)
								to_erase.push_back(&name);
							else
								amount -= chosen;
						}, "Amount to Discard");
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
			ImGui::EndTabItem();
		}

		selectedTab = -1;
		ImGui::EndTabBar();
	}

	ImGui::End();
	ImGui::PopStyleVar();
}

bool MainWindow::insert(std::shared_ptr<Area> area, const std::string &resource_name, double amount) {
	try {
		if (amount <= 0) {
			context.message = "Error: Invalid amount.";
			return false;
		}

		if (context->inventory.count(resource_name) == 0) {
			context.message = "Error: You don't have any of that resource.";
			return false;
		}

		double &in_inventory = context->inventory.at(resource_name);
		if (in_inventory < amount) {
			context.message = "Error: You don't have enough of that resource.";
			return false;
		}

		if ((in_inventory -= amount) < 0.000001)
			context->inventory.erase(resource_name);

		area->resources[resource_name] += amount;
		return true;
	} catch (const std::exception &err) {
		fprintf(stderr, "??? %s\n", err.what());
		context.message = "??? " + std::string(err.what());
		return false;
	}
}
