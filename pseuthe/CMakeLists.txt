project(pseuthe)
cmake_minimum_required(VERSION 2.8)

if(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build (Debug or Release)" FORCE)
endif()

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")
SET(PSEUTHE_STATIC_SFML FALSE CACHE BOOL "Choose whether SFML is linked statically or not.")
SET(PSEUTHE_STATIC_RUNTIME FALSE CACHE BOOL "Use statically linked standard/runtime libraries? This option must match the one used for SFML.")

#use if you prefer clang, comment out if using g++ (4.9+)
#SET(CMAKE_C_COMPILER    "/usr/bin/clang")
#SET(CMAKE_CXX_COMPILER  "/usr/bin/clang++")
#SET(CMAKE_AR            "/usr/bin/llvm-ar")
#SET(CMAKE_LINKER        "/usr/bin/llvm-ld")
#SET(CMAKE_NM            "/usr/bin/llvm-nm")
#SET(CMAKE_OBJDUMP       "/usr/bin/llvm-objdump")
#SET(CMAKE_RANLIB        "/usr/bin/llvm-ranlib")

if(CMAKE_COMPILER_IS_GNUCXX)
  if(PSEUTHE_STATIC_RUNTIME)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11 -static")
  else()
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11")
  endif()
endif()

if(PSEUTHE_STATIC_SFML)
  SET(SFML_STATIC_LIBRARIES TRUE)
endif()

if(WIN32)
  find_package(SFML 2 REQUIRED graphics window audio system main)
else()
  find_package(SFML 2 REQUIRED graphics window audio system)
endif()

if(UNIX)
  find_package(X11 REQUIRED)
endif()

#optional package manager on arch (PKGBUILD script currently missing)
#include (FindPkgConfig)
#PKG_SEARCH_MODULE(SFML 2 REQUIRED system window graphics audio)

include_directories(
  ${SFML_INCLUDE_DIR}
  ${CMAKE_SOURCE_DIR}/include)

if(X11_FOUND)
  include_directories(${X11_INCLUDE_DIRS})
endif()

#if using mingw on windows compile the icon / resource files
set(RES_FILES "")
if(MINGW)
  set(RES_FILES "icon.rc")
  set(CMAKE_RC_COMPILER_INIT windres)
  enable_language(RC)
  set(CMAKE_RC_COMPILE_OBJECT
  "<CMAKE_RC_COMPILER> <FLAGS> -O coff <DEFINES> -i <SOURCE> -o <OBJECT>")
endif()

set(SOURCE
  src/main.cpp
  src/Affectors.cpp
  src/AnimatedDrawable.cpp
  src/App.cpp
  src/AudioManager.cpp
  src/BodypartController.cpp
  src/CausticDrawable.cpp
  src/CircleDrawable.cpp
  src/Component.cpp
  src/EchoDrawable.cpp
  src/Entity.cpp
  src/FadeDrawable.cpp
  src/FileSystem.cpp
  src/FontResource.cpp
  src/GameController.cpp
  src/GameState.cpp
  src/GradientDrawable.cpp
  src/HelpState.cpp
  src/InputComponent.cpp
  src/LightPosition.cpp
  src/MenuState.cpp
  src/MessageBus.cpp
  src/Music.cpp
  src/OrbController.cpp
  src/ParticleFactory.cpp
  src/ParticleField.cpp
  src/ParticleSystem.cpp
  src/PhysicsComponent.cpp
  src/PhysicsConstraint.cpp
  src/PhysicsWorld.cpp
  src/PlanktonController.cpp
  src/PostBloom.cpp
  src/PostChromeAb.cpp
  src/PostProcess.cpp
  src/Scene.cpp
  src/Score.cpp
  src/ScoreState.cpp
  src/ShaderResource.cpp
  src/SoundPlayer.cpp
  src/State.cpp
  src/StateStack.cpp
  src/TailDrawable.cpp
  src/TailDrawablePhysics.cpp
  src/TextDrawable.cpp
  src/Ticker.cpp
  src/TitleState.cpp
  src/UIButton.cpp
  src/UICheckBox.cpp
  src/UIComboBox.cpp
  src/UIContainer.cpp
  src/UIControl.cpp
  src/UIScoreList.cpp
  src/UISelection.cpp
  src/UISlider.cpp
  src/UITextBox.cpp
  ${RES_FILES})
  
if(WIN32)
  add_executable(pseuthe WIN32 ${SOURCE})
else()
  add_executable(pseuthe ${SOURCE})
endif()

target_link_libraries(pseuthe
  ${SFML_LIBRARIES}
  ${SFML_DEPENDENCIES})

if(UNIX)
  target_link_libraries(pseuthe
    ${X11_LIBRARIES})
endif()

#install executable
install(TARGETS pseuthe
  RUNTIME DESTINATION .)

#install game data
install(DIRECTORY assets
  DESTINATION .)

#install the OpenAL DLL
if(WIN32)
  install(FILES ${SFML_INCLUDE_DIR}/../bin/openal32.dll
    DESTINATION .)
endif()
