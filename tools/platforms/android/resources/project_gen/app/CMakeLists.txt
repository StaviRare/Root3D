cmake_minimum_required(VERSION 3.22.1)

project("root3d")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(rootPath "../../../../../../")

# Define globally for all configurations
add_definitions(-DYAML_CPP_STATIC_DEFINE)

# Set per-variant log level (passed via Gradle)
add_definitions(-DENGINE_LOG_LEVEL=${ENGINE_LOG_LEVEL})

file(GLOB_RECURSE SOURCES
    "${rootPath}source/game/*.cpp"
	"${rootPath}source/app/common/*.h"
	"${rootPath}source/app/common/*.cpp"
    "${rootPath}source/app/p_android/cpp/*.cpp"
	"${rootPath}source/engine/layer_0/log/p_android/*.cpp"
	"${rootPath}source/engine/layer_0/files/p_android/*.cpp"
	"${rootPath}source/engine/layer_0/graphics/common/*.cpp"
	"${rootPath}source/engine/layer_0/graphics/p_android/*.cpp"
	"${rootPath}source/engine/layer_0/input/p_android/*.cpp"
	"${rootPath}source/engine/layer_0/physics/*.cpp"
	"${rootPath}source/engine/layer_0/platform/p_android/*.cpp"
	"${rootPath}source/engine/layer_0/screen/p_android/*.cpp"
	"${rootPath}source/engine/layer_0/timer/*.cpp"
	"${rootPath}source/engine/layer_1/*.cpp"
	"${rootPath}source/engine/layer_2/*.cpp"
	"${rootPath}external/jolt-physics-5.0.0/Jolt/*.cpp"
	"${rootPath}external/yaml-cpp-0.6.3/src/*.cpp"
	"${rootPath}external/ttf2mesh/src/*.c"
)

set(INCLUDE_DIRS
    "${rootPath}source/game/"
	"${rootPath}source/app/common/"
    "${rootPath}source/app/p_android/cpp/"
	"${rootPath}source/engine/layer_0/collections/"
	"${rootPath}source/engine/layer_0/log/common/"
	"${rootPath}source/engine/layer_0/log/p_android/"
	"${rootPath}source/engine/layer_0/files/common/"
	"${rootPath}source/engine/layer_0/files/p_android/"
	"${rootPath}source/engine/layer_0/graphics/common/"
	"${rootPath}source/engine/layer_0/graphics/p_android/"
	"${rootPath}source/engine/layer_0/input/common/"
	"${rootPath}source/engine/layer_0/input/p_android/"
	"${rootPath}source/engine/layer_0/physics/"
	"${rootPath}source/engine/layer_0/platform/common/"
	"${rootPath}source/engine/layer_0/platform/p_android/"
	"${rootPath}source/engine/layer_0/screen/common/"
	"${rootPath}source/engine/layer_0/screen/p_android/"
	"${rootPath}source/engine/layer_0/timer/"
	"${rootPath}source/engine/layer_0/types/"
	"${rootPath}source/engine/layer_1/config/"
	"${rootPath}source/engine/layer_1/rtti/"
	"${rootPath}source/engine/layer_1/math/"
	"${rootPath}source/engine/layer_1/debug/"
	"${rootPath}source/engine/layer_1/random/"
	"${rootPath}source/engine/layer_2/ecs/"
	"${rootPath}source/engine/layer_2/physics/"
	"${rootPath}source/engine/layer_2/renderer/"
	"${rootPath}source/engine/layer_2/resource"
    "${rootPath}external/stb/include/"
	"${rootPath}external/jolt-physics-5.0.0/"
	"${rootPath}external/yaml-cpp-0.6.3/include/"
	"${rootPath}external/ttf2mesh/include/"
)

include_directories(${INCLUDE_DIRS})

find_library(LOG_LIB log)
find_library(GLESv1_LIB GLESv1_CM)
find_library(EGL_LIB EGL)

add_library(native-lib SHARED ${SOURCES})

target_link_libraries(
    native-lib
    ${LOG_LIB}
    ${GLESv1_LIB}
    ${EGL_LIB}
    android
)
