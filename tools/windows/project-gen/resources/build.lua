ROOT_PATH = "../../../../"
SOURCE_DIR = ROOT_PATH .. "source/"
EXTERNAL_DIR = ROOT_PATH .. "external/"
PROJECT_DIR = ROOT_PATH .. "workspaces/windows/"
BUILD_OUTPUT_DIR = ROOT_PATH .. "build/windows/"

workspace "root3d"
    location(PROJECT_DIR)
    configurations { "Debug", "Release" }
    platforms { "x86", "x64" }

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"

    filter { "platforms:x86", "configurations:Debug" }
        targetdir(BUILD_OUTPUT_DIR .. "/x86/Debug/bin")
        objdir(BUILD_OUTPUT_DIR .. "/x86/Debug/obj")

    filter { "platforms:x86", "configurations:Release" }
        targetdir(BUILD_OUTPUT_DIR .. "/x86/Release/bin")
        objdir(BUILD_OUTPUT_DIR .. "/x86/Release/obj")

    filter { "platforms:x64", "configurations:Debug" }
        targetdir(BUILD_OUTPUT_DIR .. "/x64/Debug/bin")
        objdir(BUILD_OUTPUT_DIR .. "/x64/Debug/obj")

    filter { "platforms:x64", "configurations:Release" }
        targetdir(BUILD_OUTPUT_DIR .. "/x64/Release/bin")
        objdir(BUILD_OUTPUT_DIR .. "/x64/Release/obj")

    filter {} -- Clear filter to apply settings to all configurations and platforms

project "Root3D"
    language "C++"
    
	files {
        SOURCE_DIR .. "app/windows/**.h",
        SOURCE_DIR .. "app/windows/**.cpp",
        SOURCE_DIR .. "engine/core/**.h",
        SOURCE_DIR .. "engine/core/**.cpp",
        SOURCE_DIR .. "engine/platform/shared/**.h",
        SOURCE_DIR .. "engine/platform/shared/**.cpp",
        SOURCE_DIR .. "engine/platform/windows/**.h",
        SOURCE_DIR .. "engine/platform/windows/**.cpp",
        SOURCE_DIR .. "game/**.h",
        SOURCE_DIR .. "game/**.cpp",
        EXTERNAL_DIR .. "glew-2.1.0/src/**.c"
    }
    
	includedirs {
        SOURCE_DIR .. "engine/core/",
        SOURCE_DIR .. "engine/platform/shared/",
        SOURCE_DIR .. "engine/platform/windows/",
        SOURCE_DIR .. "game/",
        EXTERNAL_DIR .. "stb/include/",
        EXTERNAL_DIR .. "glew-2.1.0/include/"
    }
	
    links { "opengl32" }

    filter "configurations:Debug"
        kind "ConsoleApp"
        postbuildcommands {
            'copy "%{wks.location}..\\..\\assets\\packed\\resources.bin" "%{cfg.targetdir}\\"'
        }

    filter "configurations:Release"
        kind "WindowedApp"
        linkoptions { "/SUBSYSTEM:WINDOWS" }
        postbuildcommands {
            'copy "%{wks.location}..\\..\\assets\\packed\\resources.bin" "%{cfg.targetdir}\\"'
        }
