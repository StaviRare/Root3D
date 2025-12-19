#pragma once

#include "SystemPlatform.h"
#include "RuntimeSettings.h"

class Config
{
    public:
    static RuntimeSettings Runtime()
    {
        RuntimeSettings returnValue;
        Platform platform = SystemPlatform::Get();

        // MaxLights - Not yet implemented. ShaderManager will handle that.

        switch (platform)
        {
            case Platform::Windows:
                returnValue.ScreenWidth = 960;
                returnValue.ScreenHeight = 540;
                returnValue.FullScreen = true;
                returnValue.MaxLights = 20;
                returnValue.RenderingAPI = GraphicsAPI::DirectX11;
                returnValue.PhysicsTypeAPI = PhysicsType::Jolt;
            break;

            case Platform::Android:
                returnValue.ScreenWidth = 960;
                returnValue.ScreenHeight = 540;
                returnValue.FullScreen = true;
                returnValue.MaxLights = 10;
                returnValue.RenderingAPI = GraphicsAPI::OpenGLES1;
                returnValue.PhysicsTypeAPI = PhysicsType::Jolt;
            break;
        }

        return returnValue;
    }
};