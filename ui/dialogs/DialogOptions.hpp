#ifndef DialogOptions_hpp
#define DialogOptions_hpp

#include "settings/FontsList.hpp"
#include "settings/Settings.h"
#include "DialogStyle.hpp"
#include "utilities/imgui/imgui.h"

class DialogOptions {
public:
  void init();
  void showOptionsWindow(ImGuiStyle* ref, DialogStyle* wStyle, bool* p_opened = NULL, bool* needsFontChange = NULL);
  void loadFonts(bool* needsFontChange = NULL);

private:
  std::unique_ptr<FontsList> fontLister;

  int optionsFontSelected, optionsFontSizeSelected;
};

#endif /* DialogOptions_hpp */
