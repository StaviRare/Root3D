cmake_minimum_required(VERSION 3.22.1)

project("root3d")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(rootPath "../../../../../../")

file(GLOB_RECURSE SOURCES
    "${rootPath}source/game/*.cpp"
    "${rootPath}source/app/android/cpp/*.cpp"
	"${rootPath}source/engine/layer_0/debug/p_android/*.cpp"	
	"${rootPath}source/engine/layer_0/files/p_android/*.cpp"
	"${rootPath}source/engine/layer_0/graphics/common/*.cpp"
	"${rootPath}source/engine/layer_0/graphics/p_android/*.cpp"
	"${rootPath}source/engine/layer_0/input/p_android/*.cpp"	
	"${rootPath}source/engine/layer_0/physics/*.cpp"		
	"${rootPath}source/engine/layer_0/platform/p_android/*.cpp",		
	"${rootPath}source/engine/layer_0/screen/p_android/*.cpp"
	"${rootPath}source/engine/layer_0/timer/*.cpp"		
	"${rootPath}source/engine/layer_1/*.cpp"
	"${rootPath}source/engine/layer_2/*.cpp"
	"${rootPath}external/jolt-physics-5.0.0/Jolt/*.cpp"
)

set(INCLUDE_DIRS
    "${rootPath}source/game/"
    "${rootPath}source/app/android/cpp/"
	"${rootPath}source/engine/layer_0/collections/"
	"${rootPath}source/engine/layer_0/debug/common/"
	"${rootPath}source/engine/layer_0/debug/p_android/"
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
	"${rootPath}source/engine/layer_1/core/"
	"${rootPath}source/engine/layer_1/math/"
	"${rootPath}source/engine/layer_2/ecs/"
	"${rootPath}source/engine/layer_2/physics/"
	"${rootPath}source/engine/layer_2/renderer/"
	"${rootPath}source/engine/layer_2/resource"
    "${rootPath}external/stb/include/"
	"${rootPath}external/jolt-physics-5.0.0/"
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
