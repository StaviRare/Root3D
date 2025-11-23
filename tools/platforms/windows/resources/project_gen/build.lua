ROOT_PATH = "../../../../../"
SOURCE_DIR = ROOT_PATH .. "source/"
EXTERNAL_DIR = ROOT_PATH .. "external/"
EXAMPLE_DIR = ROOT_PATH .. "example/"
PROJECT_DIR = ROOT_PATH .. "workspaces/windows/"
BUILD_OUTPUT_DIR = ROOT_PATH .. "build/windows/"

workspace "root3d"
    location(PROJECT_DIR)
    configurations { "Debug", "Release" }
    platforms { "x86", "x64" }

    filter "configurations:Debug"
        defines { "DEBUG", "ENGINE_LOG_LEVEL=0", "YAML_CPP_STATIC_DEFINE" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG", "ENGINE_LOG_LEVEL=3", "YAML_CPP_STATIC_DEFINE" }
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
    cppdialect "C++17"
	
	files {
		SOURCE_DIR .. "client/main/windows/**.cpp",
		SOURCE_DIR .. "client/runtime/**.h",
		SOURCE_DIR .. "client/runtime/**.cpp",
		SOURCE_DIR .. "engine/core/**.h",
		SOURCE_DIR .. "engine/core/**.cpp",
        SOURCE_DIR .. "engine/layer_0/collections/**.h",
		SOURCE_DIR .. "engine/layer_0/log/common/**.h",
        SOURCE_DIR .. "engine/layer_0/log/p_windows/**.cpp",	
        SOURCE_DIR .. "engine/layer_0/files/common/**.h",
        SOURCE_DIR .. "engine/layer_0/files/p_windows/**.cpp",
        SOURCE_DIR .. "engine/layer_0/graphics/common/**.h",
		SOURCE_DIR .. "engine/layer_0/graphics/common/**.cpp",
		SOURCE_DIR .. "engine/layer_0/graphics/p_windows/**.h",
        SOURCE_DIR .. "engine/layer_0/graphics/p_windows/**.cpp",
        SOURCE_DIR .. "engine/layer_0/input/common/**.h",
        SOURCE_DIR .. "engine/layer_0/input/p_windows/**.cpp",		
        SOURCE_DIR .. "engine/layer_0/physics/**.h",
        SOURCE_DIR .. "engine/layer_0/physics/**.cpp",			
        SOURCE_DIR .. "engine/layer_0/platform/common/**.h",
        SOURCE_DIR .. "engine/layer_0/platform/p_windows/**.cpp",				
        SOURCE_DIR .. "engine/layer_0/screen/common/**.h",
        SOURCE_DIR .. "engine/layer_0/screen/p_windows/**.cpp",	
        SOURCE_DIR .. "engine/layer_0/timer/**.h",
        SOURCE_DIR .. "engine/layer_0/timer/**.cpp",		
		SOURCE_DIR .. "engine/layer_0/types/**.h",
		SOURCE_DIR .. "engine/layer_1/**.h",
        SOURCE_DIR .. "engine/layer_1/**.cpp",	
		SOURCE_DIR .. "engine/layer_2/**.h",
        SOURCE_DIR .. "engine/layer_2/**.cpp",	
        EXTERNAL_DIR .. "glew-2.1.0/src/**.c",
        EXTERNAL_DIR .. "jolt-physics-5.0.0/Jolt/**.cpp",
		EXTERNAL_DIR .. "yaml-cpp-0.6.3/src/**.cpp",
		EXTERNAL_DIR .. "ttf2mesh/src/**.c",
		EXAMPLE_DIR .. "**.h",
		EXAMPLE_DIR .. "**.cpp"
    }
    
	includedirs {
		SOURCE_DIR .. "client/runtime/",
		SOURCE_DIR .. "engine/core/",
        SOURCE_DIR .. "engine/layer_0/collections/",
		SOURCE_DIR .. "engine/layer_0/log/common/",
        SOURCE_DIR .. "engine/layer_0/files/common/",
        SOURCE_DIR .. "engine/layer_0/graphics/common/",
        SOURCE_DIR .. "engine/layer_0/input/common/",	
        SOURCE_DIR .. "engine/layer_0/physics/",		
        SOURCE_DIR .. "engine/layer_0/platform/common/",			
        SOURCE_DIR .. "engine/layer_0/screen/common/",
        SOURCE_DIR .. "engine/layer_0/timer/",	
		SOURCE_DIR .. "engine/layer_0/types/",
		SOURCE_DIR .. "engine/layer_1/config/",
		SOURCE_DIR .. "engine/layer_1/rtti/",
		SOURCE_DIR .. "engine/layer_1/math/",	
		SOURCE_DIR .. "engine/layer_1/debug/",		
		SOURCE_DIR .. "engine/layer_1/random/",
		SOURCE_DIR .. "engine/layer_1/objects/",
		SOURCE_DIR .. "engine/layer_2/ecs/",
		SOURCE_DIR .. "engine/layer_2/physics/",	
		SOURCE_DIR .. "engine/layer_2/renderer/",
		SOURCE_DIR .. "engine/layer_2/resource",
        EXTERNAL_DIR .. "stb/include/",
        EXTERNAL_DIR .. "glew-2.1.0/include/",
        EXTERNAL_DIR .. "jolt-physics-5.0.0/",
		EXTERNAL_DIR .. "yaml-cpp-0.6.3/include/",
		EXTERNAL_DIR .. "ttf2mesh/include/",
		EXAMPLE_DIR .. ""
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
