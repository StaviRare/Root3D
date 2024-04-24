cmake_minimum_required(VERSION 3.22.1)

# Project Name
project("root3d")

# Use globbing to find source files:
file(GLOB_RECURSE SOURCE_SHARED "../../shared/*.cpp")
file(GLOB_RECURSE SOURCE_ANDROID *.cpp)


# Define the native library that includes the source files
set(ALL_SOURCES ${SOURCE_ANDROID} ${SOURCE_SHARED})
add_library(native-lib SHARED ${ALL_SOURCES})

# Include directories for header files
include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}
        "../../shared/"
        "../../../dependencies/stb/include/"
)

# Find necessary Android libraries to link against
find_library(LOG_LIB log)
find_library(GLESv1_LIB GLESv1_CM)
find_library(EGL_LIB EGL)

# Link the libraries with the native library
target_link_libraries(
        native-lib
        ${LOG_LIB}
        ${GLESv1_LIB}
        ${EGL_LIB}
        android
)
