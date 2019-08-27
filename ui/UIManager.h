#pragma once

#include "../utilities/imgui/imgui.h"
#include "../utilities/imgui/imgui_impl_sdl.h"
#include "../utilities/imgui/imgui_impl_vulkan.h"
#include <tchar.h>
#include <memory>
#include <string>

#include "Components/Log.h"

class UIManager {
public: 
	void init();
	void renderStart();
	void renderEnd();
	
	void doLog(const std::string& message);

	bool Show_ImGui_DemoWindow, Show_ImGui_AnotherWindow;
	
private:
	ImVec4 guiClearColor;

	std::unique_ptr<Log> componentLog;

	void dialogLog();
};

