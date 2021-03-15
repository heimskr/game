#include "Game.h"
#include "Region.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "main.h"
#include "MainWindow.h"
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
#define TAB(_name, _n, _block) if (ImGui::BeginTabItem((_name), nullptr, selectedTab == (_n)? ImGuiTabItemFlags_SetSelected : 0)) { lastTab = (_n); _block; ImGui::EndTabItem(); }
		TAB("Region",      0, { renderRegion(region); });
		TAB("Travel",      1, { renderTravel(region); });
		TAB("Extractions", 2, { renderExtractions();  });
		TAB("Inventory",   3, { renderInventory();    });
		TAB("Conversion",  4, { renderConversion();   });
		TAB("Market",      5, { renderMarket(region); });
		TAB("Automation",  6, { renderAutomation();   });
		TAB("Crafting",    7, { renderCrafting();     });
#undef TAB
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
		if (ltna(in_inventory, amount)) {
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
