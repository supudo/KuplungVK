#ifndef ControlsManager_hpp
#define ControlsManager_hpp

#include "../ivulkan/VulkanIncludes.h"

struct ControlPoint {
  int x, y;
};

class ControlsManager {
public:
  ControlsManager();
  ~ControlsManager();
  void init(SDL_Window* sdlWindow);
  void processEvents(const SDL_Event* ev);

  bool gameIsRunning, keyPressed_ESC, keyPressed_DELETE;

  bool mouseButton_LEFT, mouseButton_MIDDLE, mouseButton_RIGHT;
  bool mouseGoLeft, mouseGoRight, mouseGoUp, mouseGoDown;

  bool keyPressed_LALT, keyPressed_LSHIFT, keyPressed_LCTRL;
  bool keyPressed_RALT, keyPressed_RSHIFT, keyPressed_RCTRL;
  bool keyPresset_TAB;

  int xrel, yrel;

  ControlPoint mouseWheel, mousePosition;

private:
  SDL_Window* sdlWindow;

  void handleInput(const SDL_Event* ev);
  void handleKeyDown(const SDL_Event* ev);
  void handleMouse(const SDL_Event* ev);
  void handleMouseWheel(const SDL_Event* ev);
  void handleMouseMotion(const SDL_Event* ev);
};

#endif /* ControlsManager_hpp */
