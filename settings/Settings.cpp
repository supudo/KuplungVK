#include "Settings.h"
#include <memory>
#include <stdarg.h>

#include <string>
#include <vector>
#include <Windows.h>

Settings* Settings::m_pInstance = NULL;

Settings* Settings::Instance() {
	if (!m_pInstance)
		m_pInstance = new Settings;
	return m_pInstance;
}

void Settings::initSettings(const std::string& iniFolder) {
	m_pInstance->guiClearColor = { 70.0f / 255.0f, 70.0f / 255.0f, 70.0f / 255.0f, 255.0f / 255.0f };

	m_pInstance->MainWindow_Width = 1400;
	m_pInstance->MainWindow_Height = 800;
	m_pInstance->frameLog_Width = 300;
	m_pInstance->frameLog_Height = 200;

	m_pInstance->Setting_CurrentDriveIndex = 0;
	m_pInstance->Setting_SelectedDriveIndex = 0;
	m_pInstance->hddDriveList.empty();
	DWORD drives = ::GetLogicalDrives();
	if (drives) {
		char drive[] = "?";
		int dc = 0;
		for (int i = 0; i < 26; i++) {
			if (drives & (1 << i)) {
				drive[0] = 'A' + i;
				m_pInstance->hddDriveList.push_back(drive);
				if (&Settings::Instance()->currentFolder[0] == drive) {
					m_pInstance->Setting_CurrentDriveIndex = dc;
					m_pInstance->Setting_SelectedDriveIndex = dc;
				}
			}
			dc += 1;
		}
	}
	m_pInstance->ApplicationConfigurationFolder = iniFolder;
}

void Settings::setLogFunc(const std::function<void(std::string)>& doLog) {
	this->funcDoLog = doLog;
}