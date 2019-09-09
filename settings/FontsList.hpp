#ifndef FontsList_hpp
#define FontsList_hpp

#include "Settings.h"
#include <string>
#include <vector>

class FontsList {
public:
  void init();
  bool fontFileExists(std::string const& font);
  void getFonts();
  int getSelectedFontSize();

  std::vector<FBEntity> fonts;
  const char* fontSizes[11] = {"12", "14", "16", "18", "20", "22", "24", "26", "28", "30", "32"};

private:
  void loadFontsOSX();
  void loadFontsWindows();
  void loadFontsNix();
};

#endif /* FontsList_hpp */
