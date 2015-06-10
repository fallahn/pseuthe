project(pseuthe)
cmake_minimum_required(VERSION 2.8)

#prefer clang, comment out if using g++ (4.9+)
#SET (CMAKE_C_COMPILER             "/usr/bin/clang")
SET (CMAKE_C_FLAGS                "-Wall -std=c99")
SET (CMAKE_C_FLAGS_DEBUG          "-g -_DEBUG_")
SET (CMAKE_C_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
SET (CMAKE_C_FLAGS_RELEASE        "-O4 -DNDEBUG")
SET (CMAKE_C_FLAGS_RELWITHDEBINFO "-O2 -g")

#SET (CMAKE_CXX_COMPILER             "/usr/bin/clang++")
SET (CMAKE_CXX_FLAGS                "-Wall")
SET (CMAKE_CXX_FLAGS_DEBUG          "-g -_DEBUG_")
SET (CMAKE_CXX_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
SET (CMAKE_CXX_FLAGS_RELEASE        "-O4 -DNDEBUG")
SET (CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")

#SET (CMAKE_AR      "/usr/bin/llvm-ar")
#SET (CMAKE_LINKER  "/usr/bin/llvm-ld")
#SET (CMAKE_NM      "/usr/bin/llvm-nm")
#SET (CMAKE_OBJDUMP "/usr/bin/llvm-objdump")
#SET (CMAKE_RANLIB  "/usr/bin/llvm-ranlib")
#---------------------------------------------

SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y")

SET (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

find_package(SFML 2 REQUIRED system window graphics audio)
if(UNIX)
find_package(X11 REQUIRED)
endif(UNIX)

#optional package manager on arch (PKGBUILD script currently missing)
#include (FindPkgConfig)
#PKG_SEARCH_MODULE(SFML 2 REQUIRED system window graphics audio)


include_directories(
	${SFML_INCLUDE_DIRS}
	${CMAKE_SOURCE_DIR}/include)

link_libraries(
	${SFML_LIBRARIES})

if(X11_FOUND)
include_directories(${X11_INCLUDE_DIRS})
link_libraries(${X11_LIBRARIES})
endif(X11_FOUND)

add_executable(
	pseuthe src/main.cpp
	src/App.cpp
	src/CircleDrawable.cpp
	src/Component.cpp
	src/EchoDrawable.cpp
	src/Entity.cpp
	src/FileSystem.cpp
	src/FontResource.cpp
	src/GameState.cpp
	src/MenuState.cpp
	src/MessageBus.cpp
	src/Music.cpp
	src/PauseState.cpp
	src/PhysicsComponent.cpp
	src/PhysicsWorld.cpp
	src/Scene.cpp
	src/SoundPlayer.cpp
	src/State.cpp
	src/StateStack.cpp)

#copy resources to output directory
#file(COPY ${CMAKE_SOURCE_DIR}/res DESTINATION ${CMAKE_DESTDIR})
		

