#pragma once

#include "Types.h"
#include "GraphicsAPI.h"

struct GraphicsDesc
{
    uint32 maxLights = 10;
    void* windowHandle = nullptr;
    GraphicsAPI graphicsAPI = GraphicsAPI::Null;
};