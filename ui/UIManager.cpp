#include "UIManager.hpp"
#include "settings/Settings.h"
#include "ui/iconfonts/IconsFontAwesome.h"
#include "ui/iconfonts/IconsMaterialDesign.h"

UIManager::UIManager() {
  this->showDialogStyle = false;
  this->showSceneStats = false;
  this->showOptions = false;
  this->showAppMetrics = false;
  this->showAboutImgui = false;
  this->showAboutKuplungVK = false;
  this->showDemoWindow = false;
  this->needsFontChange = false;
}

UIManager::~UIManager() {
  this->componentLog.reset();
  this->dialogOptions.reset();
  this->dialogStyle.reset();
}

void UIManager::init(SDL_Window* sdlWindow, ImGui_ImplVulkan_InitInfo init_info, ImGui_ImplVulkanH_Window* windowVulkan,
  const std::function<void()>& funcQuitApp,
  const std::function<void()>& funcNewScene) {
	Settings::Instance()->funcDoLog(Settings::Instance()->string_format("[KuplungVK + UIManager] Init!"));

  this->funcQuitApp = funcQuitApp;
  this->funcNewScene = funcNewScene;

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO(); (void)io;
  //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
  //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

  ImGui::StyleColorsDark();
  //ImGui::StyleColorsClassic();

  ImGui_ImplSDL2_InitForVulkan(sdlWindow);
  ImGui_ImplVulkan_Init(&init_info, windowVulkan->RenderPass);

	// Our state
	this->guiClearColor = ImVec4(Settings::Instance()->guiClearColor.r, Settings::Instance()->guiClearColor.g, Settings::Instance()->guiClearColor.b, Settings::Instance()->guiClearColor.w);

	this->componentLog = std::make_unique<Log>();
	this->componentLog->init(40, 40, Settings::Instance()->frameLog_Width, Settings::Instance()->frameLog_Height);

  this->dialogOptions = std::make_unique<DialogOptions>();
  this->dialogOptions->init();

  this->dialogStyle = std::make_unique<DialogStyle>();
  ImGuiStyle& style = ImGui::GetStyle();
  this->dialogStyle->saveDefault(style);
  style = this->dialogStyle->loadCurrent();

  this->dialogOptions->loadFonts(&this->needsFontChange);
}

void UIManager::renderUI(SDL_Window* sdlWindow, ImVec4 vkClearColor) {
  ImGui_ImplVulkan_NewFrame();
  ImGui_ImplSDL2_NewFrame(sdlWindow);
  ImGui::NewFrame();

  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("File")) {
      if (ImGui::MenuItem(ICON_FA_FILE_O " New"))
        this->funcNewScene();

      ImGui::Separator();

#ifdef _WIN32
      if (ImGui::MenuItem(ICON_FA_POWER_OFF " Quit", "Alt+F4"))
#else
      if (ImGui::MenuItem(ICON_FA_POWER_OFF " Quit", "Cmd+Q"))
#endif
        this->funcQuitApp();
      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("View")) {
      if (ImGui::MenuItem(ICON_FA_BUG " Show Log Window", NULL, &Settings::Instance()->logDebugInfo))
        Settings::Instance()->saveSettings();
      ImGui::MenuItem(ICON_FA_TACHOMETER " Scene Statistics", NULL, &this->showSceneStats);
      ImGui::Separator();
      ImGui::MenuItem(ICON_FA_COG " Options", NULL, &this->showOptions);
      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Help")) {
      ImGui::MenuItem(ICON_FA_INFO " Metrics", NULL, &this->showAppMetrics);
      ImGui::MenuItem(ICON_FA_INFO_CIRCLE " About ImGui", NULL, &this->showAboutImgui);
      ImGui::MenuItem(ICON_FA_INFO_CIRCLE " About Kuplung", NULL, &this->showAboutKuplungVK);
      ImGui::Separator();
      ImGui::MenuItem("   ImGui Demo Window", NULL, &this->showDemoWindow);
      ImGui::EndMenu();
    }

    ImGui::Text(" --> %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    ImGui::EndMainMenuBar();
  }

  /*if (this->showDialogStyle)
    this->renderDialogStyle();*/

  /*if (Settings::Instance()->logDebugInfo)
    this->renderDialogLog();*/

  if (this->showAppMetrics)
    this->renderDialogAppMetrics();

  if (this->showAboutImgui)
    this->renderDialogAboutImGui();

  if (this->showAboutKuplungVK)
    this->renderDialogAboutKuplungVK();

  if (this->showOptions)
    this->renderDialogOptions(&ImGui::GetStyle());

  /*if (this->showSceneStats)
    this->renderDialogSceneStats();*/

  if (this->showDemoWindow)
    ImGui::ShowTestWindow();
}

void UIManager::renderStart() {
  /*if (Settings::Instance()->logDebugInfo)
    this->dialogLog();*/
}

void UIManager::renderEnd(VkCommandBuffer command_buffer) {
  if (this->needsFontChange) {
    this->dialogOptions->loadFonts(&this->needsFontChange);
    ImGui_ImplVulkan_CreateFontsTexture(command_buffer);
  }

	ImGui::Render();
}

bool UIManager::processEvent(SDL_Event* event) {
  //return ImGui_ImplVulkan_ProcessEvent(event);
  return true;
}

void UIManager::doLog(const std::string& message) {
	printf("%s\n", message.c_str());
	this->componentLog->addToLog("%s\n", message.c_str());
}

void UIManager::renderDialogLog() {
	int windowWidth = 800, windowHeight = 600;
	int posX = windowWidth - Settings::Instance()->frameLog_Width - 10;
	int posY = windowHeight - Settings::Instance()->frameLog_Height - 10;
	this->componentLog->init(posX, posY, Settings::Instance()->frameLog_Width, Settings::Instance()->frameLog_Height);
	this->componentLog->draw("Log Window");
}

void UIManager::setSceneSelectedModelObject(int sceneSelectedModelObject) {
}

void UIManager::renderDialogAppMetrics() {
  ImGui::ShowMetricsWindow(&this->showAppMetrics);
}

void UIManager::renderDialogAboutImGui() {
  ImGui::SetNextWindowPosCenter();
  ImGui::Begin("About ImGui", &this->showAboutImgui, ImGuiWindowFlags_AlwaysAutoResize);
  ImGui::Text("ImGui %s", ImGui::GetVersion());
  ImGui::Separator();
  ImGui::Text("By Omar Cornut and all github contributors.");
  ImGui::Text("ImGui is licensed under the MIT License, see LICENSE for more information.");
  ImGui::End();
}

void UIManager::renderDialogAboutKuplungVK() {
  ImGui::SetNextWindowPosCenter();
  ImGui::Begin("About KuplungVK", &this->showAboutKuplungVK, ImGuiWindowFlags_AlwaysAutoResize);
  ImGui::Text("KuplungVK %s", Settings::Instance()->appVersion.c_str());
  ImGui::Separator();
  ImGui::Text("By supudo.net + github.com/supudo");
  ImGui::Text("Whatever license...");
  ImGui::Separator();
  ImGui::Text("Hold mouse wheel to rotate around");
  ImGui::Text("Left Alt + Mouse wheel to increase/decrease the FOV");
  ImGui::Text("Left Shift + Mouse wheel to increase/decrease the FOV");
  ImGui::End();
}

void UIManager::renderDialogOptions(ImGuiStyle* ref) {
  this->dialogOptions->showOptionsWindow(ref, this->dialogStyle.get(), &this->showOptions, &this->needsFontChange);
}
