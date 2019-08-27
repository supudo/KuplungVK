#pragma once

#include "../../utilities/imgui/imgui.h"

class Log {
public:
	void init(int positionX, int positionY, int width, int height);
	void clear();
	void addToLog(const char* fmt, ...) IM_FMTARGS(2);
	void draw(const char* title, bool* p_opened = NULL);

private:
	int positionX, positionY, width, height;

	ImGuiTextBuffer Buf;
	ImGuiTextFilter Filter;
	ImVector<int> LineOffsets;
	bool ScrollToBottom;
};

