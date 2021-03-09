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
		consoleUpdate(console);
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
	// ImGui::StyleColorsClassic();

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
		UI::addFont(fonts_std.address, fonts_std.size, 27.0f, &config, io.Fonts->GetGlyphRangesCyrillic());
		strcpy(config.Name, "Nintendo Standard Big");
		UI::addFont(fonts_std.address, fonts_std.size, 36.0f, &config, io.Fonts->GetGlyphRangesCyrillic());

		strcpy(config.Name, "Nintendo Ext");
		static const ImWchar ranges[] = {
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
	time_t last_time = getTime();
	MainWindow main_window(context);

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
				if (event.cbutton.which == 0 && event.cbutton.button == SDL_CONTROLLER_BUTTON_START) {
					SDL_Event quitEvent;
					quitEvent.type = SDL_QUIT;
					SDL_PushEvent(&quitEvent);
				}
				ImGui_ImplSDL2_ProcessEvent(&event);
			} else {
				ImGui_ImplSDL2_ProcessEvent(&event);
			}
		}

		time_t new_time = getTime();
		for (s64 i = 0; i < new_time - last_time; ++i)
			context->tick();
		last_time = new_time;

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame(window);
		ImGui::NewFrame();

		try {
			main_window.render(&show_main_window);
			if (!context.message.empty())
				ImGui::OpenPopup("Message");
		} catch (const std::exception &err) {
			context.message = "Error: " + std::string(err.what());
			ImGui::OpenPopup("Message");
		}

		if (context.showResourcePicker) {
			constexpr float MODAL_WIDTH = 600.f, MODAL_HEIGHT = 300.f;
			ImGui::SetNextWindowPos(ImVec2((1280.f - MODAL_WIDTH) / 2.f, (720.f - MODAL_HEIGHT) / 2.f), ImGuiCond_Once);
			ImGui::SetNextWindowSize(ImVec2(MODAL_WIDTH, MODAL_HEIGHT), ImGuiCond_Once);
			ImGui::OpenPopup("Resource Selector");
			bool modal_open = true;
			if (ImGui::BeginPopupModal("Resource Selector", &modal_open, 0 & (ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))) {
				const float width = ImGui::GetContentRegionMax().x;
				if (ImGui::BeginTable("Resources", 2)) {
					ImGui::TableSetupColumn("Resource", ImGuiTableColumnFlags_WidthFixed, 200.f);
					ImGui::TableSetupColumn("Amount",   ImGuiTableColumnFlags_WidthFixed, width - 200.f);
					for (const auto &[name, amount]: context->inventory) {
						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
						if (ImGui::Selectable(name.c_str())) {
							context.onResourcePicked(name);
							context.showResourcePicker = false;
							ImGui::CloseCurrentPopup();
						}
						ImGui::TableNextColumn();
						ImGui::TableSetColumnIndex(1);
						ImGui::Text("%.2f", amount);
						ImGui::TableNextColumn();
					}
					ImGui::EndTable();
				}
				ImGui::EndPopup();
			}
			if (!modal_open)
				context.showResourcePicker = false;
		} else {
			constexpr float MODAL_WIDTH = 600.f, MODAL_HEIGHT = 250.f;
			ImGui::SetNextWindowPos(ImVec2((1280.f - MODAL_WIDTH) / 2.f, (720.f - MODAL_HEIGHT) / 2.f), ImGuiCond_Always);
			ImGui::SetNextWindowSize(ImVec2(MODAL_WIDTH, MODAL_HEIGHT), ImGuiCond_Always);
			if (ImGui::BeginPopupModal("Message", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove)) {
				auto &style = ImGui::GetStyle();
				ImGui::BeginChild("message contents",
					{600.f - style.WindowPadding.x * 2 - style.FrameBorderSize, 220.f - (style.WindowPadding.y + style.IndentSpacing) * 2},
					false, ImGuiWindowFlags_HorizontalScrollbar);
				ImGui::Text("%s", context.message.c_str());
				ImGui::EndChild();
				if (ImGui::Button("Close")) {
					context.message.clear();
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}
		}

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		SDL_GL_SwapWindow(window);
	}

	ImGui::DestroyContext();

	SDL_GL_DeleteContext(gl_context);
	SDL_DestroyWindow(window);
	SDL_Quit();

	socketExit();
	return EXIT_SUCCESS;
}

time_t getTime() {
	time_t current_time;
	Result result = timeGetCurrentTime(TimeType_UserSystemClock, (u64 *) &current_time);
	if (R_FAILED(result)) {
		fprintf(stderr, "Couldn't get current time.\n");
		exit(EXIT_FAILURE);
	}
	return current_time;
}

void Context::pickResource(std::function<void(const std::string &)> fn) {
	showResourcePicker = true;
	onResourcePicked = fn;
}

void Context::load() {
	try {
		game = Game::load();
		print("Game loaded successfully.\n");
		loaded = true;
	} catch (const std::exception &err) {
		print("Couldn't load game: %s\n", err.what());
		message = "Couldn't load game: " + std::string(err.what());
	}
}

void Context::save() {
	if (!game) {
		message = "No game is open.";
		printf("No game.\n");
		return;
	}

	try {
		game->save();
		message = "Game saved successfully.";
		printf("Game saved.\n");
	} catch (const std::exception &err) {
		message = "An error occurred while saving: " + std::string(err.what());
		printf("Game save error: %s\n", err.what());
	}
}
