#undef __APPLE__

#include <switch.h>

#include "imgui.h"
#include "imgui_impl_glfw_switch.h"
#include "imgui_impl_opengl3.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#define GLM_FORCE_PURE
#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "Game.h"
#include "main.h"
#include "MainWindow.h"

#include <cstdio>
#include <cstdlib>
#include <chrono>
#include <memory>

constexpr uint32_t WINDOW_WIDTH  = 1280;
constexpr uint32_t WINDOW_HEIGHT = 720;

int main() {
	socketInitializeDefault();
	nxlinkStdio();
	std::srand(std::time(nullptr));
	glfwSetErrorCallback(+[](int error_code, const char *description) {
		fprintf(stderr, "GLFW error %d: %s\n", error_code, description);
	});

	if (!glfwInit()) {
		fprintf(stderr, "glfwInit returned 0\n");
		return EXIT_FAILURE;
	}

	const char *glsl_version = "#version 430 core";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "test", nullptr, nullptr);
	if (!window) {
		fprintf(stderr, "window is nullptr\n");
		glfwTerminate();
		return EXIT_FAILURE;
	}

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);
	glfwMakeContextCurrent(window);

	// Load OpenGL routines using glad
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	glfwSwapInterval(1);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();

	io.ConfigFlags |= ImGuiConfigFlags_IsTouchScreen;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableSetMousePos;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	// ImGui::StyleColorsClassic();

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	// Load Fonts
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
		io.Fonts->AddFontFromMemoryTTF(fonts_std.address, fonts_std.size, 24.0f, &config, io.Fonts->GetGlyphRangesCyrillic());

		strcpy(config.Name, "Nintendo Ext");
		static const ImWchar ranges[] = {
			0xE000, 0xE06B,
			0xE070, 0xE07E,
			0xE080, 0xE099,
			0xE0A0, 0xE0BA,
			0xE0C0, 0xE0D6,
			0xE0E0, 0xE0F5,
			0xE100, 0xE105,
			0xE110, 0xE116,
			0xE121, 0xE12C,
			0xE130, 0xE13C,
			0xE140, 0xE14D,
			0xE150, 0xE153,
			0,
		};

		io.Fonts->AddFontFromMemoryTTF(fonts_ext.address, fonts_ext.size, 24.0f, &config, ranges);
		io.Fonts->Build();

		plExit();
	}

	bool show_main_window = true;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	ImGuiStyle &style = ImGui::GetStyle();
	style.WindowRounding = 8;
	style.FrameRounding = style.GrabRounding = 3;

	for (int i = 0; i < io.Fonts->Fonts.Size; ++i) {
		if (strcmp(io.Fonts->Fonts[i]->GetDebugName(), "Nintendo Standard") == 0) {
			io.FontDefault = io.Fonts->Fonts[i];
			break;
		}
	}

	Context context;
	context.game = std::make_shared<Game>();

	while (!glfwWindowShouldClose(window) && show_main_window) {
		glfwPollEvents();
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		MainWindow(context, &show_main_window);

		if (!context.message.empty()) {
			ImGui::OpenPopup("Message");
		}

		constexpr float MODAL_WIDTH = 500.f, MODAL_HEIGHT = 200.f;
		ImGui::SetNextWindowPos(ImVec2((1280.f - MODAL_WIDTH) / 2.f, (720.f - MODAL_HEIGHT) / 2.f), ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2(MODAL_WIDTH, MODAL_HEIGHT), ImGuiCond_Always);
		if (ImGui::BeginPopupModal("Message", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove)) {
			ImGui::Text("%s", context.message.c_str());
			if (ImGui::Button("Close")) {
				context.message.clear();
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}

		ImGui::Render();
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();

	socketExit();
	return EXIT_SUCCESS;
}

void Context::save() {
	if (!game) {
		message = "No game is open.";
		printf("No game.\n");
		return;
	}

	try {
		// game->save();
		message = "Game saved successfully.";
		printf("Game saved.\n");
	} catch (const std::exception &err) {
		message = "An error occurred while saving: " + std::string(err.what());
		printf("Game save error: %s\n", err.what());
		return;
	}
}
