#pragma once

#include "Vector3.h"
#include "Physics.h"
#include "Graphics.h"

struct RuntimeSettings
{
    // Window
    int ScreenWidth = -1;
    int ScreenHeight = -1;
    bool FullScreen = true;

    // Graphics
    int MaxLights = 10;
    GraphicsAPI RenderingAPI = GraphicsAPI::Null;

    // Physics
    Vector3 Gravity = Vector3(0, -9.81f, 0);
    PhysicsType PhysicsTypeAPI = PhysicsType::Null;
};