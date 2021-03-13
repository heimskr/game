#include "Game.h"
#include "imgui.h"
#include "Keyboard.h"
#include "main.h"
#include "MainWindow.h"
#include "Stonks.h"
#include "area/Housing.h"

void MainWindow::renderMarket(Region *region) {
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
								if (chosen <= 0. || ltna(amount, chosen)) {
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
								if (chosen < 0. || ltna(amount, chosen)) {
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
}
