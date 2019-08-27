#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <type_traits>
#include <typeinfo>
#include <memory>
#include <functional>
#include "SettingsStructs.h"

class Settings {
public:
	static Settings* Instance();
	void initSettings(const std::string& iniFolder);
	void saveSettings();
  std::string string_format(const std::string& fmt_str, ...);
	std::string appFolder();
	void setLogFunc(const std::function<void(std::string)>& doLog);

	std::function<void(std::string)> funcDoLog;
	
	int MainWindow_Width, MainWindow_Height;
	int frameLog_Width, frameLog_Height;
	
	std::string ApplicationConfigurationFolder, currentFolder;
	int Setting_CurrentDriveIndex, Setting_SelectedDriveIndex;
	std::vector<std::string> hddDriveList;

	Color guiClearColor;

private:
	Settings() {}
	Settings(Settings const&) {}
	Settings& operator=(Settings const&);
	static Settings* m_pInstance;
};
