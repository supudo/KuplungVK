#ifndef RenderingManager_hpp
#define RenderingManager_hpp

#include "utilities/ivulkan/VulkanIncludes.h"
#include "settings/Settings.h"

class RenderingManager {
public:
  RenderingManager();
  ~RenderingManager();
  void init();
  void render();

private:
};

#endif /* RenderingManager_hpp */
