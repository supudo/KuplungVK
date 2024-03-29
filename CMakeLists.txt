CMAKE_MINIMUM_REQUIRED(VERSION 3.5)

SET(PROJECT_NAME "KuplungVK")
PROJECT(${PROJECT_NAME})

IF(CMAKE_BUILD_TYPE STREQUAL Debug)
  SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/cmake-build/Debug")
  SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/cmake-build/Debug")
  SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/cmake-build/Debug")
ELSE(CMAKE_BUILD_TYPE STREQUAL Debug)
  SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/cmake-build/Release")
  SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/cmake-build/Release")
  SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/cmake-build/Release")
ENDIF(CMAKE_BUILD_TYPE STREQUAL Debug)

ADD_DEFINITIONS(-DDEBUG)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -stdlib=libc++ -Os")

#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DIMGUI_IMPL_OPENGL_LOADER_CUSTOM")

IF((CMAKE_CXX_COMPILER_ID MATCHES GNU) OR (CMAKE_CXX_COMPILER_ID MATCHES Clang))
  SET(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3")
  SET(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
ENDIF()

IF(APPLE)
  SET(MACOSX_BUNDLE true)
  SET(MACOSX_BUNDLE_BUNDLE_NAME ${MY_APP_NAME})
  SET(MACOSX_BUNDLE_INFO_PLIST, "${CMAKE_SOURCE_DIR}/Info.plist")
ENDIF(APPLE)

# Libraries
IF(APPLE)
  INCLUDE_DIRECTORIES(/System/Library/Frameworks)
  FIND_LIBRARY(COCOA_LIBRARY Cocoa)
  MARK_AS_ADVANCED(COCOA_LIBRARY)
  FIND_LIBRARY(VULKAN_LIBRARY Vulkan)
  MARK_AS_ADVANCED(VULKAN_LIBRARY)
ELSE(APPLE)
  MESSAGE(FATAL_ERROR "Cocoa Libraries not found!")
ENDIF(APPLE)

# Boost
FIND_PACKAGE(Boost COMPONENTS system filesystem REQUIRED)
IF(Boost_FOUND)
  INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
ENDIF(Boost_FOUND)

# GLM
INCLUDE(cmake/FindGLM.cmake)
IF(GLM_FOUND)
  INCLUDE_DIRECTORIES(${GLM_INCLUDE_DIRS})
ELSE(GLM_FOUND)
  MESSAGE(FATAL_ERROR "GLM not found!")
ENDIF(GLM_FOUND)

# SDL2
FIND_PACKAGE(SDL2)
IF(SDL2_FOUND)
  INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIRS})
ELSE(SDL2_FOUND)
  MESSAGE(FATAL_ERROR "SDL2 not found!")
ENDIF(SDL2_FOUND)

# GLFW3
FIND_PACKAGE(GLFW3 REQUIRED)
IF(GLFW3_FOUND)
  INCLUDE_DIRECTORIES(${GLFW3_INCLUDE_DIRS})
ELSE(GLFW3_FOUND)
  MESSAGE(FATAL_ERROR "GLFW3 not found!")
ENDIF(GLFW3_FOUND)

# Assimp
INCLUDE(cmake/Findassimp.cmake)
IF(assimp_FOUND)
  INCLUDE_DIRECTORIES(${assimp_INCLUDE_DIRS})
ELSE(assimp_FOUND)
  MESSAGE(FATAL_ERROR "Assimp not found!")
ENDIF(assimp_FOUND)

# Protobuf
FIND_PACKAGE(Protobuf REQUIRED)
IF(Protobuf_FOUND)
INCLUDE_DIRECTORIES(${Protobuf_INCLUDE_DIRS})
ELSE(Protobuf_FOUND)
MESSAGE(FATAL_ERROR "Google ProtoBuf not found!")
ENDIF(Protobuf_FOUND)

# Minizip
INCLUDE(cmake/FindMinizip.cmake)
IF(MINIZIP_FOUND)
  INCLUDE_DIRECTORIES(${MINIZIP_INCLUDE_DIR})
ELSE(MINIZIP_FOUND)
  MESSAGE(FATAL_ERROR "Minizip not found!")
ENDIF(MINIZIP_FOUND)

INCLUDE_DIRECTORIES(.)

ADD_DEFINITIONS(-DGLM_ENABLE_EXPERIMENTAL)

SET(SOURCE_FILES
  utilities/ivulkan/VulkanIncludes.h
  utilities/imgui/imconfig.h
  utilities/imgui/imgui_demo.cpp
  utilities/imgui/imgui_draw.cpp
  utilities/imgui/imgui_impl_sdl.cpp
  utilities/imgui/imgui_impl_sdl.h
  utilities/imgui/imgui_impl_vulkan.cpp
  utilities/imgui/imgui_impl_vulkan.h
  utilities/imgui/imgui_internal.h
  utilities/imgui/imgui_widgets.cpp
  utilities/imgui/imgui.cpp
  utilities/imgui/imgui.h
  utilities/imgui/imstb_rectpack.h
  utilities/imgui/imstb_textedit.h
  utilities/imgui/imstb_truetype.h
  utilities/controls/ControlsManager.hpp
  utilities/controls/ControlsManager.cpp
  settings/SettingsStructs.h
  settings/Settings.h
  settings/Settings.cpp
  settings/ConfigUtils.hpp
  settings/ConfigUtils.cpp
  settings/FontsList.hpp
  settings/FontsList.cpp
  ui/UIManager.hpp
  ui/UIManager.cpp
  ui/iconfonts/IconsFontAwesome.h
  ui/iconfonts/IconsMaterialDesign.h
  ui/components/Log.h
  ui/components/Log.cpp
  ui/dialogs/DialogOptions.hpp
  ui/dialogs/DialogOptions.cpp
  ui/dialogs/DialogStyle.hpp
  ui/dialogs/DialogStyle.cpp
  rendering/RenderingManager.hpp
  rendering/RenderingManager.cpp
  Kuplung.hpp
  Kuplung.cpp
  main.cpp)

SET(KuplungVK_Misc
  resources/KuplungVK.icns
  resources/KuplungVK.ico
  resources/KuplungVK_RecentFiles.ini
  resources/KuplungVK_RecentFilesImported.ini
  resources/KuplungVK_Settings.ini)

SET(KuplungVK_Fonts
  resources/fonts/fontawesome-webfont.ttf
  resources/fonts/material-icons-regular.ttf)

IF (Boost_FOUND AND GLM_FOUND AND SDL2_FOUND AND GLFW3_FOUND AND assimp_FOUND AND Protobuf_FOUND AND MINIZIP_FOUND)
  IF (APPLE)
    INCLUDE(CMakeToolsHelpers OPTIONAL)

    ADD_EXECUTABLE(${PROJECT_NAME} MACOSX_BUNDLE ${SOURCE_FILES})

    # Misc Files
    EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/resources/" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}.app/Contents/Resources/")
    FILE(GLOB KuplungVK_Ini_Files "resources/*.ini")
    FILE(COPY ${KuplungVK_Ini_Files} DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}.app/Contents/Resources/")
    FILE(GLOB KuplungVK_Icns_Files "resources/*.icns")
    FILE(COPY ${KuplungVK_Icns_Files} DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}.app/Contents/Resources/")
    FILE(GLOB KuplungVK_Ico_Files "resources/*.ico")
    FILE(COPY ${KuplungVK_Ico_Files} DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}.app/Contents/Resources/")

    SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES MACOSX_BUNDLE_INFO_PLIST "${CMAKE_SOURCE_DIR}/Info.plist")

    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${COCOA_LIBRARY})
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${VULKAN_LIBRARY})
  ENDIF(APPLE)
ELSE(Boost_FOUND AND GLM_FOUND AND SDL2_FOUND AND GLFW3_FOUND AND assimp_FOUND AND Protobuf_FOUND AND MINIZIP_FOUND)
  MESSAGE(FATAL_ERROR "Some of the required libraries were not found. Please check the log.")
ENDIF(Boost_FOUND AND GLM_FOUND AND SDL2_FOUND AND GLFW3_FOUND AND assimp_FOUND AND Protobuf_FOUND AND MINIZIP_FOUND)

IF(Boost_FOUND)
  TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${Boost_LIBRARIES})
ENDIF(Boost_FOUND)

IF(GLM_FOUND)
  TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${GLM_LIBRARIES})
ENDIF(GLM_FOUND)

IF(SDL2_FOUND)
  STRING(STRIP ${SDL2_LIBRARIES} SDL2_LIBRARIES)
  TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${SDL2_LIBRARIES})
ENDIF(SDL2_FOUND)

IF(GLFW3_FOUND)
  TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${GLFW3_LIBRARIES})
ENDIF(GLFW3_FOUND)

IF(assimp_FOUND)
  TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${assimp_LIBRARIES})
ENDIF(assimp_FOUND)

IF(Protobuf_FOUND)
  TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${Protobuf_LIBRARIES})
ENDIF(Protobuf_FOUND)

IF(MINIZIP_FOUND)
  TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${MINIZIP_LIBRARIES})
ENDIF(MINIZIP_FOUND)

