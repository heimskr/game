#undef __APPLE__

#include <SDL2/SDL.h>
#include <switch.h>

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

#define GLFW_INCLUDE_NONE
#include <glad/glad.h>

#define GLM_FORCE_PURE
#define GLM_ENABLE_EXPERIMENTAL

#include "FS.h"
#include "Game.h"
#include "main.h"
#include "MainWindow.h"
#include "UI.h"

#include <cstdio>
#include <cstdlib>
#include <chrono>
#include <memory>
#include <sstream>

constexpr uint32_t WINDOW_WIDTH  = 1280;
constexpr uint32_t WINDOW_HEIGHT = 720;

int main() {
	try {
		socketInitializeDefault();
		nxlinkStdio();
		std::srand(std::time(nullptr));

		if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER) < 0)
			throw std::runtime_error(SDL_GetError());

		FS::init();
	} catch (const std::exception &err) {
		consoleInit(nullptr);
		printf("Error: %s\n", err.what());
		consoleUpdate(nullptr);
		PadState pad;
		padConfigureInput(1, HidNpadStyleSet_NpadStandard);
		padInitializeDefault(&pad);
		while (appletMainLoop()) {
			padUpdate(&pad);
			u64 kDown = padGetButtonsDown(&pad);
			if (kDown & HidNpadButton_Plus)
				break;
		}
		socketExit();
		consoleExit(nullptr);
		exit(EXIT_FAILURE);
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

	int w = 1280, h = 720;
	SDL_Window *window = SDL_CreateWindow("TradeGame", 0, 0, w, h, SDL_WINDOW_OPENGL);
	if (!window)
		throw std::runtime_error(SDL_GetError());

	SDL_GLContext gl_context = SDL_GL_CreateContext(window);
	SDL_SetWindowSize(window, w, h);
	SDL_GL_MakeCurrent(window, gl_context);
	SDL_GL_SetSwapInterval(1);

	if (gladLoadGL() == 0)
		throw std::runtime_error("gladLoadGL failed");

	if (SDL_GameControllerOpen(0) == nullptr)
		throw std::runtime_error(SDL_GetError());


	SDL_Event event;
	bool done = false;

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();

	io.ConfigFlags |= ImGuiConfigFlags_IsTouchScreen;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableSetMousePos;
	// io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	io.MouseDrawCursor = true;

	if (!ImGui_ImplSDL2_InitForOpenGL(window, gl_context, true)) {
		throw std::runtime_error("ImGui_ImplSDL2_InitForOpenGL failed");
	}

	if (!ImGui_ImplOpenGL3_Init("#version 330 core")) {
		throw std::runtime_error("ImGui_ImplOpenGL3_Init failed");
	}

	ImGui::StyleColorsDark();

	io.Fonts->AddFontDefault();
	{
		plInitialize(PlServiceType_System);

		PlFontData fonts_std;
		PlFontData fonts_ext;

		plGetSharedFontByType(&fonts_std, PlSharedFontType_Standard);
		plGetSharedFontByType(&fonts_ext, PlSharedFontType_NintendoExt);

		ImFontConfig config;
		config.FontDataOwnedByAtlas = false;

		strcpy(config.Name, "Nintendo Standard");
		UI::addFont(fonts_std.address, fonts_std.size, 27.0f, &config, io.Fonts->GetGlyphRangesDefault());
		UI::addFont(fonts_ext.address, fonts_ext.size, 27.0f, &config, io.Fonts->GetGlyphRangesDefault());
		strcpy(config.Name, "Nintendo Standard Big");
		UI::addFont(fonts_std.address, fonts_std.size, 36.0f, &config, io.Fonts->GetGlyphRangesDefault());
		UI::addFont(fonts_ext.address, fonts_ext.size, 36.0f, &config, io.Fonts->GetGlyphRangesDefault());

		strcpy(config.Name, "Nintendo Ext");
		static const ImWchar ranges[] = {
			0x2070, 0x209c,
			0xe000, 0xe06b,
			0xe070, 0xe07e,
			0xe080, 0xe099,
			0xe0a0, 0xe0ba,
			0xe0c0, 0xe0d6,
			0xe0e0, 0xe0f5,
			0xe100, 0xe105,
			0xe110, 0xe116,
			0xe121, 0xe12c,
			0xe130, 0xe13c,
			0xe140, 0xe14d,
			0xe150, 0xe153,
			0,
		};

		io.Fonts->AddFontFromMemoryTTF(fonts_ext.address, fonts_ext.size, 24.0f, &config, ranges);
		io.Fonts->Build();

		plExit();
	}

	ImGuiStyle &style = ImGui::GetStyle();
	style.WindowRounding = 8;
	style.FrameRounding = style.GrabRounding = 3;

	io.FontDefault = UI::getFont("Nintendo Standard");

	Context context;
	context.game = std::make_shared<Game>();

	bool show_main_window = true;
	auto last_time = getTime();
	MainWindow main_window(context);

	bool show_demo = false;

	while (show_main_window && !done) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				done = true;
			} else if (event.type == SDL_CONTROLLERAXISMOTION) {
				const bool up = event.caxis.value == 0;
				if (up && event.caxis.which == 0) {
					if (event.caxis.axis == 4) { // ZL
						if (main_window.lastTab == 0)
							main_window.selectedTab = MainWindow::TAB_COUNT - 1;
						else
							main_window.selectedTab = main_window.lastTab - 1;
					} else if (event.caxis.axis == 5) { // ZR
						if (main_window.lastTab == MainWindow::TAB_COUNT - 1)
							main_window.selectedTab = 0;
						else
							main_window.selectedTab = main_window.lastTab + 1;
					}
				}
				ImGui_ImplSDL2_ProcessEvent(&event);
			} else if (event.type == SDL_CONTROLLERBUTTONDOWN) {
				if (event.cbutton.which == 0) {
					const u8 button = event.cbutton.button;
					if (button == SDL_CONTROLLER_BUTTON_START) {
						SDL_Event quitEvent;
						quitEvent.type = SDL_QUIT;
						SDL_PushEvent(&quitEvent);
					} else if (button == SDL_CONTROLLER_BUTTON_DPAD_RIGHT)
						context.rightDown = true;
					else if (button == SDL_CONTROLLER_BUTTON_DPAD_DOWN)
						context.downDown = true;
					else if (button == SDL_CONTROLLER_BUTTON_DPAD_LEFT)
						context.leftDown = true;
					else if (button == SDL_CONTROLLER_BUTTON_DPAD_UP)
						context.upDown = true;
				}
				ImGui_ImplSDL2_ProcessEvent(&event);
			} else if (event.type == SDL_CONTROLLERBUTTONUP) {
				if (event.cbutton.which == 0) {
					const u8 button = event.cbutton.button;
					if (button == SDL_CONTROLLER_BUTTON_DPAD_RIGHT)
						context.rightDown = false;
					else if (button == SDL_CONTROLLER_BUTTON_DPAD_DOWN)
						context.downDown = false;
					else if (button == SDL_CONTROLLER_BUTTON_DPAD_LEFT)
						context.leftDown = false;
					else if (button == SDL_CONTROLLER_BUTTON_DPAD_UP)
						context.upDown = false;
				}
				ImGui_ImplSDL2_ProcessEvent(&event);
			} else {
				ImGui_ImplSDL2_ProcessEvent(&event);
			}
		}

		auto new_time = getTime();
		s64 count = (new_time - last_time).count();
		constexpr s64 divisor = 100;
		// Up to 1,000 seconds of time spent in the background will be simulated on return.
		if (context.loaded)
			for (s64 i = 0; i < count / divisor && i < 1'000'000 / divisor; ++i)
				context->tick(0.001 * divisor);
		if (count / divisor)
			last_time = new_time;

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame(window);
		ImGui::NewFrame();

		try {
			main_window.render(&show_main_window);
			if (!context.message.empty())
				ImGui::OpenPopup("Message");
		} catch (const std::exception &err) {
			Logger::error("Error: %s", err.what());
			context.showMessage("Error: " + std::string(err.what()));
			ImGui::OpenPopup("Message");
		}

		if (show_demo)
			ImGui::ShowDemoWindow(&show_demo);

		if (context.showResourcePicker) {
			constexpr float MODAL_WIDTH = 600.f, MODAL_HEIGHT = 300.f;
			ImGui::SetNextWindowPos(ImVec2((1280.f - MODAL_WIDTH) / 2.f, (720.f - MODAL_HEIGHT) / 2.f), ImGuiCond_Once);
			ImGui::SetNextWindowSize(ImVec2(MODAL_WIDTH, MODAL_HEIGHT), ImGuiCond_Once);
			ImGui::OpenPopup("Resource Selector");
			bool modal_open = true;
			if (ImGui::BeginPopupModal("Resource Selector", &modal_open, 0 & (ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))) {
				for (const auto &[name, resource]: context->resources)
					if (ImGui::Selectable(name.c_str())) {
						context.onResourcePicked(name);
						context.showResourcePicker = false;
						ImGui::CloseCurrentPopup();
						break;
					}
				ImGui::EndPopup();
			}
			if (!modal_open)
				context.showResourcePicker = false;
		} else if (context.showProcessorTypePicker) {
			constexpr float MODAL_WIDTH = 600.f, MODAL_HEIGHT = 300.f;
			ImGui::SetNextWindowPos(ImVec2((1280.f - MODAL_WIDTH) / 2.f, (720.f - MODAL_HEIGHT) / 2.f), ImGuiCond_Once);
			ImGui::SetNextWindowSize(ImVec2(MODAL_WIDTH, MODAL_HEIGHT), ImGuiCond_Once);
			ImGui::OpenPopup("Processor Type Selector");
			bool modal_open = true;
			if (ImGui::BeginPopupModal("Processor Type Selector", &modal_open, 0 & (ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))) {
				for (Processor::Type type: Processor::TYPES) {
					if (ImGui::Selectable(Processor::typeName(type))) {
						context.onProcessorTypePicked(type);
						context.showProcessorTypePicker = false;
						ImGui::CloseCurrentPopup();
						break;
					}
					if (ImGui::IsItemHovered() && context->processorCosts.count(type) != 0) {
						std::stringstream ss;
						bool first = true;
						for (const auto &[name, amount]: context->processorCosts.at(type)) {
							if (first)
								first = false;
							else
								ss << "\n";
							ss << name << " x " << std::to_string(amount);
						}
						ImGui::SetTooltip("%s", ss.str().c_str());
					}
				}
				ImGui::EndPopup();
			}
			if (!modal_open)
				context.showProcessorTypePicker = false;
		} else if (context.showInventoryPicker) {
			constexpr float MODAL_WIDTH = 900.f, MODAL_HEIGHT = 300.f;
			ImGui::SetNextWindowPos(ImVec2((1280.f - MODAL_WIDTH) / 2.f, (720.f - MODAL_HEIGHT) / 2.f), ImGuiCond_Once);
			ImGui::SetNextWindowSize(ImVec2(MODAL_WIDTH, MODAL_HEIGHT), ImGuiCond_Once);
			ImGui::OpenPopup("Inventory Selector");
			bool modal_open = true;
			if (ImGui::BeginPopupModal("Inventory Selector", &modal_open, 0 & (ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))) {
				if (ImGui::BeginTable("Inventory", 2)) {
					ImGui::TableSetupColumn("Resource", ImGuiTableColumnFlags_WidthStretch);
					ImGui::TableSetupColumn("Amount", ImGuiTableColumnFlags_WidthFixed, 300.f);
					ImGui::TableHeadersRow();
					for (const auto &[name, amount]: context->inventory) {
						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
						if (ImGui::Selectable(name.c_str())) {
							context.onInventoryPicked(name);
							context.showInventoryPicker = false;
							ImGui::CloseCurrentPopup();
							break;
						}
						ImGui::TableNextColumn();
						ImGui::TableSetColumnIndex(1);
						ImGui::Text("%.3f", amount);
						ImGui::TableNextColumn();
					}
					ImGui::EndTable();
				}
				ImGui::EndPopup();
			}
			if (!modal_open)
				context.showInventoryPicker = false;
		} else if (context.showProcessorPicker) {
			constexpr float MODAL_WIDTH = 900.f, MODAL_HEIGHT = 600.f;
			ImGui::SetNextWindowPos(ImVec2((1280.f - MODAL_WIDTH) / 2.f, (720.f - MODAL_HEIGHT) / 2.f), ImGuiCond_Once);
			ImGui::SetNextWindowSize(ImVec2(MODAL_WIDTH, MODAL_HEIGHT), ImGuiCond_Once);
			ImGui::OpenPopup("Processor Selector");
			bool modal_open = true;
			if (ImGui::BeginPopupModal("Processor Selector", &modal_open, 0 & (ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))) {
				if (!context.processorPickerMessage.empty())
					ImGui::Text("%s", context.processorPickerMessage.c_str());
				if (ImGui::BeginTable("Processor Table", 2)) {
					ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
					ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, 300.f);
					ImGui::TableHeadersRow();
					u64 i = 0;
					for (const auto &processor: context->processors) {
						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
						if (ImGui::Selectable((processor->name + "##processor_" + std::to_string(++i)).c_str())) {
							context.onProcessorPicked(processor);
							context.showProcessorPicker = false;
							ImGui::CloseCurrentPopup();
							break;
						}
						ImGui::TableNextColumn();
						ImGui::TableSetColumnIndex(1);
						ImGui::Text("%s", Processor::typeName(processor->getType()));
						ImGui::TableNextColumn();
					}
					ImGui::EndTable();
				}
				ImGui::EndPopup();
			}
			if (!modal_open)
				context.showProcessorPicker = false;
		} else if (context.showRefineryModePicker) {
			constexpr float MODAL_WIDTH = 600.f, MODAL_HEIGHT = 300.f;
			ImGui::SetNextWindowPos(ImVec2((1280.f - MODAL_WIDTH) / 2.f, (720.f - MODAL_HEIGHT) / 2.f), ImGuiCond_Once);
			ImGui::SetNextWindowSize(ImVec2(MODAL_WIDTH, MODAL_HEIGHT), ImGuiCond_Once);
			ImGui::OpenPopup("Refinery Mode Selector");
			bool modal_open = true;
			if (ImGui::BeginPopupModal("Refinery Mode Selector", &modal_open, 0 & (ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))) {
				if (!context.refineryModePickerMessage.empty())
					ImGui::Text("%s", context.refineryModePickerMessage.c_str());
				for (const RefineryMode mode: refineryModes)
					if (ImGui::Selectable(stringify(mode))) {
						context.onRefineryModePicked(mode);
						context.showRefineryModePicker = false;
						ImGui::CloseCurrentPopup();
						break;
					}
				ImGui::EndPopup();
			}
			if (!modal_open)
				context.showRefineryModePicker = false;
		}

		constexpr float MODAL_HEIGHT = 250.f;
		const float modalWidth = std::max(200.f, std::min(ImGui::CalcTextSize(context.message.c_str()).x + 20.f, 1200.f));

		ImGui::SetNextWindowPos(ImVec2((1280.f - modalWidth) / 2.f, (720.f - MODAL_HEIGHT) / 2.f), ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2(modalWidth, MODAL_HEIGHT), ImGuiCond_Always);
		if (ImGui::BeginPopupModal("Message", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove)) {
			ImVec2 max = ImGui::GetContentRegionMax();
			const float child_x = max.x - ImGui::GetStyle().WindowPadding.x;
			ImGui::BeginChild("message contents", {child_x, max.y - 80.f}, false, ImGuiWindowFlags_HorizontalScrollbar);
			ImGui::Text("%s", context.message.c_str());
			ImGui::EndChild();
			if (context.isConfirm) {
				if (ImGui::Button("Okay") || context.rightDown) {
					context.message.clear();
					ImGui::CloseCurrentPopup();
					context.onChoice(true);
				}
				ImGui::SameLine();
				if (ImGui::Button("Cancel") || context.downDown) {
					context.message.clear();
					ImGui::CloseCurrentPopup();
					context.onChoice(false);
				}
			} else if (ImGui::Button("Close") || context.rightDown || context.downDown) {
				context.message.clear();
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		SDL_GL_SwapWindow(window);

		for (const auto &fn: context.frameActions)
			fn();
		context.frameActions.clear();
	}

	ImGui::DestroyContext();

	SDL_GL_DeleteContext(gl_context);
	SDL_DestroyWindow(window);
	SDL_Quit();

	socketExit();
	return EXIT_SUCCESS;
}

std::chrono::milliseconds getTime() {
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
}

void Context::pickResource(std::function<void(const std::string &)> fn) {
	showResourcePicker = true;
	onResourcePicked = fn;
}

void Context::pickAreaType(std::function<void(Area::Type)> fn) {
	showAreaTypePicker = true;
	onAreaTypePicked = fn;
}

void Context::pickProcessorType(std::function<void(Processor::Type)> fn) {
	showProcessorTypePicker = true;
	onProcessorTypePicked = fn;
}

void Context::pickInventory(std::function<void(const std::string &)> fn) {
	showInventoryPicker = true;
	onInventoryPicked = fn;
}

void Context::pickProcessor(std::function<void(std::shared_ptr<Processor>)> fn, const std::string &message_) {
	showProcessorPicker = true;
	onProcessorPicked = fn;
	processorPickerMessage = message_;
}

void Context::pickRefineryMode(std::function<void(RefineryMode)> fn, const std::string &message_) {
	showRefineryModePicker = true;
	onRefineryModePicked = fn;
	refineryModePickerMessage = message_;
}

void Context::confirm(const std::string &str, std::function<void(bool)> fn) {
	isConfirm = true;
	message = str;
	onChoice = fn;
}

void Context::showMessage(const std::string &str) {
	isConfirm = false;
	message = str;
}

void Context::newGame() {
	game = std::make_shared<Game>();
	game->loadDefaults();
	loaded = true;
}

void Context::load() {
	try {
		game = Game::load();
		print("Game loaded successfully.\n");
		loaded = true;
	} catch (const std::exception &err) {
		print("Couldn't load game: %s\n", err.what());
		showMessage("Couldn't load game: " + std::string(err.what()));
	}
}

void Context::save() {
	if (!game) {
		showMessage("No game is open.");
		printf("No game.\n");
		return;
	}

	try {
		game->save();
		showMessage("Game saved successfully.");
		printf("Game saved.\n");
	} catch (const std::exception &err) {
		showMessage("An error occurred while saving: " + std::string(err.what()));
		printf("Game save error: %s\n", err.what());
	}
}
