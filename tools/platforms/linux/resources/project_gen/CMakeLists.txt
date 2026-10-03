cmake_minimum_required(VERSION 3.22.1)

project("root3d")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(rootPath "../../")

# Define globally for all configurations
add_definitions(-DYAML_CPP_STATIC_DEFINE)

# Set per-configuration log level
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	add_definitions(-DDEBUG)
	add_definitions(-DENGINE_LOG_LEVEL=0)
else()
	add_definitions(-DNDEBUG)
	add_definitions(-DENGINE_LOG_LEVEL=3)
endif()

file(GLOB_RECURSE SOURCES
		"${rootPath}source/client/main/linux/*.cpp"
		"${rootPath}source/client/runtime/*.h"
		"${rootPath}source/client/runtime/*.cpp"
		"${rootPath}source/engine/core/*.h"
		"${rootPath}source/engine/core/*.cpp"
		"${rootPath}source/engine/layer_0/collections/*.h"
		"${rootPath}source/engine/layer_0/log/common/*.h"
		"${rootPath}source/engine/layer_0/log/p_linux/*.cpp"
		"${rootPath}source/engine/layer_0/files/common/*.h"
		"${rootPath}source/engine/layer_0/files/p_linux/*.cpp"
		"${rootPath}source/engine/layer_0/graphics/common/*.h"
		"${rootPath}source/engine/layer_0/graphics/common/*.cpp"
		"${rootPath}source/engine/layer_0/graphics/p_linux/*.h"
		"${rootPath}source/engine/layer_0/graphics/p_linux/*.cpp"
		"${rootPath}source/engine/layer_0/input/common/*.h"
		"${rootPath}source/engine/layer_0/input/p_linux/*.cpp"
		"${rootPath}source/engine/layer_0/physics/*.h"
		"${rootPath}source/engine/layer_0/physics/*.cpp"
		"${rootPath}source/engine/layer_0/platform/common/*.h"
		"${rootPath}source/engine/layer_0/platform/common/*.cpp"
		"${rootPath}source/engine/layer_0/platform/p_linux/*.cpp"
		"${rootPath}source/engine/layer_0/window/common/*.h"
		"${rootPath}source/engine/layer_0/window/p_linux/*h"
		"${rootPath}source/engine/layer_0/window/p_linux/*.cpp"
		"${rootPath}source/engine/layer_0/timer/*.h"
		"${rootPath}source/engine/layer_0/timer/*.cpp"
		"${rootPath}source/engine/layer_0/types/*.h"
		"${rootPath}source/engine/layer_1/*.h"
		"${rootPath}source/engine/layer_1/*.cpp"
		"${rootPath}source/engine/layer_2/*.h"
		"${rootPath}source/engine/layer_2/*.cpp"
		"${rootPath}external/jolt-physics-5.0.0/Jolt/*.cpp"
		"${rootPath}external/yaml-cpp-0.6.3/src/*.cpp"
		"${rootPath}external/ttf2mesh/src/*.c"
		"${rootPath}external/wayland/src/*.c"
		"${rootPath}example/*.h"
		"${rootPath}example/*.cpp"
)

set(INCLUDE_DIRS
		"${rootPath}source/client/runtime/"
		"${rootPath}source/client/main/linux/"
		"${rootPath}source/engine/core/"
		"${rootPath}source/engine/layer_0/collections/"
		"${rootPath}source/engine/layer_0/log/common/"
		"${rootPath}source/engine/layer_0/log/p_linux/"
		"${rootPath}source/engine/layer_0/files/common/"
		"${rootPath}source/engine/layer_0/files/p_linux/"
		"${rootPath}source/engine/layer_0/graphics/common/"
		"${rootPath}source/engine/layer_0/graphics/p_linux/"
		"${rootPath}source/engine/layer_0/input/common/"
		"${rootPath}source/engine/layer_0/input/p_linux/"
		"${rootPath}source/engine/layer_0/physics/"
		"${rootPath}source/engine/layer_0/platform/common/"
		"${rootPath}source/engine/layer_0/platform/p_linux/"
		"${rootPath}source/engine/layer_0/window/common/"
		"${rootPath}source/engine/layer_0/timer/"
		"${rootPath}source/engine/layer_0/types/"
		"${rootPath}source/engine/layer_1/config/"
		"${rootPath}source/engine/layer_1/rtti/"
		"${rootPath}source/engine/layer_1/math/"
		"${rootPath}source/engine/layer_1/debug/"
		"${rootPath}source/engine/layer_1/random/"
		"${rootPath}source/engine/layer_1/objects/"
		"${rootPath}source/engine/layer_1/resource/"
		"${rootPath}source/engine/layer_2/ecs/"
		"${rootPath}source/engine/layer_2/physics/"
		"${rootPath}source/engine/layer_2/renderer/"
		"${rootPath}external/stb/include/"
		"${rootPath}external/jolt-physics-5.0.0/"
		"${rootPath}external/yaml-cpp-0.6.3/include/"
		"${rootPath}external/ttf2mesh/include/"
		"${rootPath}external/wayland/include/"
		"${rootPath}example/"
)

find_library(EGL_LIB EGL REQUIRED)
find_library(GLESV2_LIB GLESv2 REQUIRED)
find_library(WAYLAND_EGL_LIB wayland-egl REQUIRED)
find_library(WAYLAND_CLIENT_LIB wayland-client REQUIRED)

include_directories(${INCLUDE_DIRS})

add_executable(Root3D ${SOURCES})

add_custom_command(
		TARGET Root3D POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy
		"${CMAKE_CURRENT_SOURCE_DIR}/../../assets/packed/resources.bin"
		"$<TARGET_FILE_DIR:Root3D>/resources.bin"
)

target_link_libraries(
		Root3D
		${EGL_LIB}
		${GLESV2_LIB}
		${WAYLAND_CLIENT_LIB}
		${WAYLAND_EGL_LIB}
)
