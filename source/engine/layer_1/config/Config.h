#pragma once

#include "Vector3.h"
#include "Physics.h"
#include "Graphics.h"
#include "PlatformDetector.h"

struct RuntimeSettings
{
    // Window
    int ScreenWidth = -1;
    int ScreenHeight = -1;
    bool FullScreen = true;

    // Graphics
    int MaxLights = 10;
    GraphicsType RenderingAPI = GraphicsType::Null;

    // Physics
    Vector3 Gravity = Vector3(0, -9.81f, 0);
    PhysicsType PhysicsTypeAPI = PhysicsType::Null;
};

class Config
{
    public:
    static RuntimeSettings Runtime()
    {
        RuntimeSettings returnValue;

        auto platform = PlatformDetector::GetPlatform();

        // MaxLights - Not yet implemented. ShaderManager will handle that.

        switch (platform)
        {
            case Platform::Windows:
                returnValue.ScreenWidth = 960;
                returnValue.ScreenHeight = 540;
                returnValue.MaxLights = 20;
                returnValue.RenderingAPI = GraphicsType::DirectX11;
                returnValue.PhysicsTypeAPI = PhysicsType::Jolt;
            break;

            case Platform::Android:
                returnValue.ScreenWidth = 960;
                returnValue.ScreenHeight = 540;
                returnValue.MaxLights = 10;
                returnValue.RenderingAPI = GraphicsType::OpenGLES1;
                returnValue.PhysicsTypeAPI = PhysicsType::Jolt;
            break;
        }

        return returnValue;
    }
};