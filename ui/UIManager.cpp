#include "UIManager.h"
#include "../settings/Settings.h"

void UIManager::init() {
	Settings::Instance()->funcDoLog(Settings::Instance()->string_format("[KuplungVK + UIManager] Init!"));

	// Our state
	this->guiClearColor = ImVec4(Settings::Instance()->guiClearColor.r, Settings::Instance()->guiClearColor.g, Settings::Instance()->guiClearColor.b, Settings::Instance()->guiClearColor.w);

	this->Show_ImGui_AnotherWindow = true;
	this->Show_ImGui_DemoWindow = true;

	this->componentLog = std::make_unique<Log>();
	this->componentLog->init(40, 40, Settings::Instance()->frameLog_Width, Settings::Instance()->frameLog_Height);
}

void UIManager::renderStart() {
	Settings::Instance()->funcDoLog(Settings::Instance()->string_format("[UIManager] renderStart..."));
	
}

void UIManager::renderEnd() {
	/*ImGui::Render();
	memcpy(&wd->ClearValue.color.float32[0], &this->guiClearColor, 4 * sizeof(float));
	FrameRender(wd);
	FramePresent(wd);*/
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