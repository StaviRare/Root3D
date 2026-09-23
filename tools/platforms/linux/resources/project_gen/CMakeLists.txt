cmake_minimum_required(VERSION 3.22.1)

project("root3d")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(root3d
		main.cpp
		xdg-decoration-client-protocol.h
		xdg-decoration-protocol.c
		xdg-shell-client-protocol.h
		xdg-shell-protocol.c
)

find_library(EGL_LIB EGL REQUIRED)
find_library(GLESV2_LIB GLESv2 REQUIRED)
find_library(WAYLAND_EGL_LIB wayland-egl REQUIRED)
find_library(WAYLAND_CLIENT_LIB wayland-client REQUIRED)

target_link_libraries(
		root3d
		${WAYLAND_CLIENT_LIB}
		${WAYLAND_EGL_LIB}
		${EGL_LIB}
		${GLESV2_LIB}
)