cmake_minimum_required(VERSION 3.22.1)

project("root3d")

set(rootPath "../../../../../../")

file(GLOB_RECURSE SOURCES
    "${rootPath}source/app/android/cpp/*.cpp"
    "${rootPath}source/engine/core/*.cpp"
    "${rootPath}source/engine/platform/shared/*.cpp"
    "${rootPath}source/engine/platform/android/*.cpp"
    "${rootPath}source/game/*.cpp"
)

set(INCLUDE_DIRS
    "${rootPath}source/app/android/cpp/"
    "${rootPath}source/engine/core/"
    "${rootPath}source/engine/platform/shared/"
    "${rootPath}source/engine/platform/android/"
    "${rootPath}source/game/"
    "${rootPath}external/stb/include/"
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
