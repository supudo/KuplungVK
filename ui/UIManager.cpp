#include "UIManager.hpp"
#include "../settings/Settings.h"

void UIManager::init(SDL_Window* sdlWindow, ImGui_ImplVulkan_InitInfo init_info, ImGui_ImplVulkanH_Window* wd) {
	Settings::Instance()->funcDoLog(Settings::Instance()->string_format("[KuplungVK + UIManager] Init!"));

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO(); (void)io;
  //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
  //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();
  //ImGui::StyleColorsClassic();

  // Setup Platform/Renderer bindings
  ImGui_ImplSDL2_InitForVulkan(sdlWindow);
  ImGui_ImplVulkan_Init(&init_info, wd->RenderPass);

  // Load Fonts
  // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
  // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
  // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
  // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
  // - Read 'misc/fonts/README.txt' for more instructions and details.
  // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
  //io.Fonts->AddFontDefault();
  //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
  //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
  //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
  //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
  //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
  //IM_ASSERT(font != NULL);

	// Our state
	this->guiClearColor = ImVec4(Settings::Instance()->guiClearColor.r, Settings::Instance()->guiClearColor.g, Settings::Instance()->guiClearColor.b, Settings::Instance()->guiClearColor.w);

	this->Show_ImGui_AnotherWindow = true;
	this->Show_ImGui_DemoWindow = true;

	this->componentLog = std::make_unique<Log>();
	this->componentLog->init(40, 40, Settings::Instance()->frameLog_Width, Settings::Instance()->frameLog_Height);
}

void UIManager::renderStart() {
  /*if (Settings::Instance()->logDebugInfo)
    this->dialogLog();*/
}

void UIManager::renderEnd() {
	/*ImGui::Render();
	memcpy(&wd->ClearValue.color.float32[0], &this->guiClearColor, 4 * sizeof(float));
	FrameRender(wd);
	FramePresent(wd);*/
}

bool UIManager::processEvent(SDL_Event* event) {
  //return ImGui_ImplVulkan_ProcessEvent(event);
  return true;
}

void UIManager::doLog(const std::string& message) {
	printf("%s\n", message.c_str());
	this->componentLog->addToLog("%s\n", message.c_str());
}

void UIManager::dialogLog() {
	int windowWidth = 800, windowHeight = 600;
	int posX = windowWidth - Settings::Instance()->frameLog_Width - 10;
	int posY = windowHeight - Settings::Instance()->frameLog_Height - 10;
	this->componentLog->init(posX, posY, Settings::Instance()->frameLog_Width, Settings::Instance()->frameLog_Height);
	this->componentLog->draw("Log Window");
}
