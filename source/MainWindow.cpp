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
#include "Stonks.h"
#include "area/Areas.h"

void MainWindow::render(bool *open) {
	ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(1280.0f, 720.0f), ImGuiCond_Once);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);

	if (ImGui::Begin("Trade Game", open, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse)) {
		if (ImGui::BeginMenuBar()) {
			if (ImGui::BeginMenu("Menu")) {
				if (ImGui::MenuItem("New"))
					context.newGame();
				if (ImGui::MenuItem("Load"))
					context.load();
				if (context.loaded && ImGui::MenuItem("Stringify")) {
					if (context.game)
						context.showMessage(context.game->toString());
					else
						context.showMessage("Game is null");
				}
				if (context.loaded && ImGui::MenuItem("Cheats", nullptr, context->cheatsEnabled))
					context->cheatsEnabled = !context->cheatsEnabled;
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
										if (amount <= 0)
											context.showMessage("Invalid amount.");
										else if (amount < chosen)
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
				s64 i = 0;
				for (auto iter = context->extractions.begin(), end = context->extractions.end(); iter != end; ++iter) {
					const Extraction &extraction = *iter;
					if (ImGui::Button(("x##" + std::to_string(++i)).c_str()))
						context.frameActions.push_back([this, iter]() { context->extractions.erase(iter); });
					ImGui::SameLine();
					ImGui::Text("Extracting %s from %s in %s (%.2f left) @ %.2f/s",
						extraction.resourceName.c_str(), extraction.area->name.c_str(),
						extraction.area->parent->name.c_str(), extraction.amount, extraction.rate);
					if (0. < extraction.startAmount) {
						static char buf[64];
						snprintf(buf, 64, "%.3f/%.3f", extraction.startAmount - extraction.amount, extraction.startAmount);
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
			} else {
				if (context->cheatsEnabled) {
					if (ImGui::Button("+##add_resource", {40.f, 0.f}))
						context.pickResource([this](const std::string &name) {
							Keyboard::openForDouble([this, &name](double chosen) {
								if (chosen <= 0.)
									context.showMessage("Invalid amount.");
								else
									context->inventory[name] += chosen;
							});
						});
					if (ImGui::IsItemHovered())
						ImGui::SetTooltip("Add a resource into your inventory (cheat)");
				}
				if (context->inventory.empty()) {
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
									context.showMessage("Error: Invalid amount.");
								else if (amount <= chosen)
									to_erase.push_back(&name);
								else
									amount -= chosen;
							}, "Amount to Discard");
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip("Discard resource");
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
			}
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Market", nullptr, selectedTab == 4? ImGuiTabItemFlags_SetSelected : 0)) {
			lastTab = 4;
			if (!context.game || !context.loaded) {
				ImGui::Text("No game is loaded.");
			} else if (!region) {
				ImGui::Text("The void has no market.");
			} else {
				std::shared_ptr<HousingArea> housing = region->getHousing();
				if (!housing) {
					ImGui::Text((region->name + " has no market.").c_str());
				} else {
					if (context->cheatsEnabled) {
						ImGui::Text("Greed:");
						ImGui::SameLine();
						if (ImGui::Selectable((std::to_string(region->greed) + "##region_greed").c_str()))
							Keyboard::openForDouble([region](double chosen) {
								region->greed = chosen;
							}, "Region Greed");
						ImGui::Text("Region money:");
						ImGui::SameLine();
						if (ImGui::Selectable((std::to_string(region->money) + "##region_money").c_str()))
							Keyboard::openForNumber([region](size_t chosen) {
								region->money = chosen;
							}, "Amount of Money");
						ImGui::Text("Your money:");
						ImGui::SameLine();
						if (ImGui::Selectable((std::to_string(context->money) + "##player_money").c_str()))
							Keyboard::openForNumber([this](size_t chosen) {
								context->money = chosen;
							}, "Amount of Money");
					} else {
						ImGui::Text(("Region money: " + std::to_string(region->money)).c_str());
						ImGui::Text(("Your money: " + std::to_string(context->money)).c_str());
					}
					if (ImGui::BeginTable("Market Layout", 2)) {
						const float width = ImGui::GetContentRegionMax().x;
						ImGui::TableSetupColumn("Player", ImGuiTableColumnFlags_WidthFixed, width / 2.f);
						ImGui::TableSetupColumn("Region", ImGuiTableColumnFlags_WidthFixed, width / 2.f);

						const Resource::Map non_owned = region->allNonOwnedResources();
						const double greed = region->greed;
						const size_t money = region->money;

						const float half_width = ImGui::GetContentRegionMax().x / 2.;
						static char str[32];

						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
						if (ImGui::BeginTable("Player Table", 4)) {
							const float column_width = half_width / 3.f - 50.f;
							ImGui::TableSetupColumn("##sell", ImGuiTableColumnFlags_WidthFixed, 50.f);
							ImGui::TableSetupColumn("Your Resources", ImGuiTableColumnFlags_WidthStretch);
							ImGui::TableSetupColumn("Amount##player", ImGuiTableColumnFlags_WidthFixed, column_width);
							ImGui::TableSetupColumn("Price##player", ImGuiTableColumnFlags_WidthFixed, column_width);
							ImGui::TableHeadersRow();
							for (const auto &[name, amount]: context->inventory) {
								ImGui::TableNextRow();
								ImGui::TableSetColumnIndex(0);
								if (ImGui::Button(("S##" + name).c_str(), {40.f, 0.f})) {
									Keyboard::openForDouble([&](double chosen) {
										if (chosen <= 0. || amount < chosen) {
											context.showMessage("Error: Invalid amount.");
										} else {
											const double original_chosen = chosen;
											size_t total_price;
											if (!Stonks::totalSellPrice(*region, name, chosen, total_price)) {
												context.showMessage("Error: Region doesn't have enough money.\nPrice: "
													+ std::to_string(total_price));
											} else {
												context.confirm("Price: " + std::to_string(total_price), [=](bool confirmed) {
													if (confirmed) {
														housing->resources[name] += original_chosen;
														context->inventory[name] -= original_chosen;
														region->money -= total_price;
														context->money += total_price;
													}
												});
											}
										}
									}, "Amount to Sell");
								}
								ImGui::TableNextColumn();
								ImGui::TableSetColumnIndex(1);
								ImGui::Text(name.c_str());
								ImGui::TableNextColumn();
								ImGui::TableSetColumnIndex(2);
								snprintf(str, 32, "%.4f", amount);
								ImGui::Text(str);
								ImGui::TableNextColumn();
								ImGui::TableSetColumnIndex(3);
								double sell;
								try {
									sell = Stonks::sellPrice(context->resources.at(name).basePrice, non_owned.count(name)? non_owned.at(name) : 0, money, greed);
									snprintf(str, 32, "%.4f", sell);
									ImGui::Text(str);
								} catch (const std::exception &err) {
									ImGui::Text(err.what());
								}
								ImGui::TableNextColumn();
							}

							ImGui::EndTable();
						}

						ImGui::TableNextColumn();
						ImGui::TableSetColumnIndex(1);
						if (ImGui::BeginTable("Region Table", 4)) {
							const float column_width = half_width / 3.f - 50.f;
							ImGui::TableSetupColumn("Region Resources", ImGuiTableColumnFlags_WidthStretch);
							ImGui::TableSetupColumn("Amount##region", ImGuiTableColumnFlags_WidthFixed, column_width);
							ImGui::TableSetupColumn("Price##region", ImGuiTableColumnFlags_WidthFixed, column_width);
							ImGui::TableSetupColumn("##Buy", ImGuiTableColumnFlags_WidthFixed, 50.f);
							ImGui::TableHeadersRow();
							for (const auto &[name, amount]: non_owned) {
								ImGui::TableNextRow();
								ImGui::TableSetColumnIndex(0);
								ImGui::Text(name.c_str());
								ImGui::TableNextColumn();
								ImGui::TableSetColumnIndex(1);
								snprintf(str, 32, "%.4f", amount);
								ImGui::Text(str);
								ImGui::TableNextColumn();
								ImGui::TableSetColumnIndex(2);
								snprintf(str, 32, "%.4f", Stonks::buyPrice(context->resources.at(name).basePrice, amount, money));
								ImGui::Text(str);
								ImGui::TableNextColumn();
								ImGui::TableSetColumnIndex(3);
								if (ImGui::Button(("B##" + name).c_str(), {40.f, 0.f})) {
									Keyboard::openForDouble([&](double chosen) {
										if (chosen < 0. || amount < chosen) {
											context.showMessage("Error: Invalid amount.");
										} else {
											const double original_chosen = chosen;
											const size_t total_price = Stonks::totalBuyPrice(*region, name, chosen);
											if (context->money < total_price) {
												context.showMessage("Error: You don't have enough money.\nPrice: "
													+ std::to_string(total_price));
											} else {
												context.confirm("Price: " + std::to_string(total_price), [=](bool confirmed) {
													if (confirmed) {
														region->subtractResourceFromNonOwned(name, original_chosen);
														context->inventory[name] += original_chosen;
														region->money += total_price;
														context->money -= total_price;
													}
												});
											}
										}
									}, "Amount to Buy");
								}
								ImGui::TableNextColumn();
							}
							ImGui::EndTable();
						}

						ImGui::TableNextColumn();
						ImGui::EndTable();
					}
				}
			}
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Conversion", nullptr, selectedTab == 5? ImGuiTabItemFlags_SetSelected : 0)) {
			lastTab = 5;
			if (!context.game || !context.loaded) {
				ImGui::Text("The void doesn't support conversion.");
			} else  {
				if (ImGui::Button("Add Processor"))
					context.pickProcessorType([this](Processor::Type type) {
						if (context->processorCosts.count(type) == 0) {
							context.showMessage("There is no recipe for " + std::string(Processor::typeName(type))
								+ " processors.");
							return;
						}
						const Resource::Map &cost = context->processorCosts.at(type);
						if (!contains(context->inventory, cost)) {
							context.showMessage("You don't have enough resources to make that.");
							return;
						}
						for (const auto &[name, amount]: cost)
							context->inventory[name] -= amount;
						shrink(context->inventory);
						context->processors.push_back(std::unique_ptr<Processor>(Processor::ofType(*context.game, type)));
						context.showMessage("Added a new " + std::string(Processor::typeName(type)) + ".");
					});

				if (context->processors.empty()) {
					ImGui::Text("You have no processors.");
				} else {
					u64 i = 0;
					for (const std::unique_ptr<Processor> &processor: context->processors) {
						++i;
						ImGui::Text("%s", Processor::typeName(processor->getType()));
						ImGui::SameLine();
						if (ImGui::Button(("+##" + std::to_string(i)).c_str()))
							context.pickInventory([this, &processor](const std::string &name) {
								if (context->inventory.count(name) == 0) {
									context.showMessage("You don't have any " + name + ".");
									return;
								}
								Keyboard::openForDouble([this, &processor, &name](double chosen) {
									if (chosen <= 0) {
										context.showMessage("Invalid amount.");
									} else if (context->inventory[name] < chosen) {
										context.showMessage("You don't have enough " + name + ".");
									} else {
										context->inventory[name] -= chosen;
										shrink(context->inventory, name);
										processor->input[name] += chosen;
									}
								}, "Resource Amount");
							});
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip("Add resource to processor");
						if (ImGui::BeginTable(("Layout##" + std::to_string(i)).c_str(), 2)) {
							const float width = ImGui::GetContentRegionMax().x / 2.f;
							ImGui::TableSetupColumn("##input_table", ImGuiTableColumnFlags_WidthFixed, width);
							ImGui::TableSetupColumn("##output_table", ImGuiTableColumnFlags_WidthFixed, width);
							ImGui::TableNextRow();
							ImGui::TableSetColumnIndex(0);
							if (ImGui::BeginTable(("##input" + std::to_string(++i)).c_str(), 2)) {
								ImGui::TableSetupColumn("Input Resource", ImGuiTableColumnFlags_WidthStretch);
								ImGui::TableSetupColumn("Amount##input", ImGuiTableColumnFlags_WidthFixed, 300.f);
								ImGui::TableHeadersRow();
								for (const auto &[name, amount]: processor->input) {
									ImGui::TableNextRow();
									ImGui::TableSetColumnIndex(0);
									ImGui::Text("%s", name.c_str());
									ImGui::TableNextColumn();
									ImGui::TableSetColumnIndex(1);
									ImGui::Text("%.3f", amount);
									ImGui::TableNextColumn();
								}
								ImGui::EndTable();
							}
							ImGui::TableNextColumn();
							ImGui::TableSetColumnIndex(1);
							if (ImGui::BeginTable(("##output" + std::to_string(++i)).c_str(), 2)) {
								ImGui::TableSetupColumn("Output Resource", ImGuiTableColumnFlags_WidthStretch);
								ImGui::TableSetupColumn("Amount##output", ImGuiTableColumnFlags_WidthFixed, 300.f);
								ImGui::TableHeadersRow();
								u64 j = 0;
								for (auto &[name, amount]: processor->output) {
									ImGui::TableNextRow();
									ImGui::TableSetColumnIndex(0);
									if (ImGui::Selectable((name + "##outsel_" + std::to_string(++j)).c_str()))
										Keyboard::openForDouble([this, &processor, &name, &amount](double chosen) {
											if (chosen <= 0) {
												context.showMessage("Invalid amount.");
											} else if (amount < chosen) {
												context.showMessage("There isn't enough of that resource.");
											} else {
												amount -= chosen;
												context->inventory[name] += chosen;
												context.frameActions.push_back([&processor, &name] {
													shrink(processor->output, name);
												});
											}
										}, "Amount to Remove");
									ImGui::TableNextColumn();
									ImGui::TableSetColumnIndex(1);
									ImGui::Text("%.3f", amount);
									ImGui::TableNextColumn();
								}
								ImGui::EndTable();
							}
							ImGui::TableNextColumn();
							ImGui::EndTable();
						}
					}
				}
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
			context.showMessage("Error: Invalid amount.");
			return false;
		}

		if (context->inventory.count(resource_name) == 0) {
			context.showMessage("Error: You don't have any of that resource.");
			return false;
		}

		double &in_inventory = context->inventory.at(resource_name);
		if (in_inventory < amount) {
			context.showMessage("Error: You don't have enough of that resource.");
			return false;
		}

		if ((in_inventory -= amount) < Resource::MIN_AMOUNT)
			context->inventory.erase(resource_name);

		area->resources[resource_name] += amount;
		return true;
	} catch (const std::exception &err) {
		fprintf(stderr, "??? %s\n", err.what());
		context.showMessage("??? " + std::string(err.what()));
		return false;
	}
}
