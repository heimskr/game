#include "Game.h"
#include "imgui.h"
#include "main.h"
#include "MainWindow.h"

void MainWindow::renderExtractions() {
	if (context->extractions.empty()) {
		ImGui::Text("Nothing is happening.");
	} else {
		s64 i = 0;
		for (auto iter = context->extractions.begin(), end = context->extractions.end(); iter != end; ++iter) {
			const Extraction &extraction = *iter;
			if (ImGui::Button(("x##" + std::to_string(++i)).c_str()))
				context.frameActions.push_back([this, iter]() { context->extractions.erase(iter); });
			ImGui::SameLine();
			if (0. < extraction.startAmount) {
				ImGui::Text("Extracting %s from %s in %s (%.2f left) @ %.2f/s",
					extraction.resourceName.c_str(), extraction.area->name.c_str(),
					extraction.area->parent->name.c_str(), extraction.amount, extraction.rate);
				static char buf[64];
				snprintf(buf, 64, "%.3f/%.3f", extraction.startAmount - extraction.amount, extraction.startAmount);
				ImGui::ProgressBar((extraction.startAmount - extraction.amount) / extraction.startAmount, {0, 0}, buf);
			} else {
				ImGui::Text("Extracting %s from %s in %s @ %.2f/s",
					extraction.resourceName.c_str(), extraction.area->name.c_str(),
					extraction.area->parent->name.c_str(), extraction.rate);
			}
		}
	}
}
