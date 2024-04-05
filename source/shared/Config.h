#pragma once

#include "Graphics.h"

struct AppConfig
{
    // Screen:
    int ScreenWidth;
    int ScreenHeight;

    // Grpahics
    int MaxLights;
    APIType RenderingAPI;
};

class Config
{
    public:
    static AppConfig Application()
    {
        AppConfig returnValue;
        returnValue.ScreenWidth = 960;
        returnValue.ScreenHeight = 540;
        returnValue.MaxLights = 20; // Not yet implemented.
        returnValue.RenderingAPI = APIType::DirectX11;

        return returnValue;
    }
};