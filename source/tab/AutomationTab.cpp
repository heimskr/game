#include "Game.h"
#include "imgui.h"
#include "Keyboard.h"
#include "main.h"
#include "MainWindow.h"

void MainWindow::renderAutomation() {
	if (!context.game || !context.loaded) {
		ImGui::Text("There's nothing to automate in the void.");
		return;
	}

	if (ImGui::Button("+", {34.f, 0.f})) {
		context.showMessage("Add automation");
	}
}
