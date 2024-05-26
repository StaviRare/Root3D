#pragma once

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
    APIType RenderingAPI = APIType::Null;

    // Physics
    float Gravity = -9.81f;
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
                returnValue.RenderingAPI = APIType::DirectX11;
            break;

            case Platform::Android:
                returnValue.ScreenWidth = 960;
                returnValue.ScreenHeight = 540;
                returnValue.MaxLights = 10;
                returnValue.RenderingAPI = APIType::OpenGLES1;
            break;
        }

        return returnValue;
    }
};