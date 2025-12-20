#pragma once

#include "Types.h"
#include "GraphicsAPI.h"

struct GraphicsConfig
{
    uint32 MaxLights = 10;
    GraphicsAPI RenderingAPI = GraphicsAPI::Null;
    void* windowHandle = nullptr;
};
