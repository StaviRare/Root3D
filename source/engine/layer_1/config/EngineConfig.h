#pragma once

#include "PhysicsConfig.h"
#include "GraphicsConfig.h"
#include "WindowConfig.h"

struct EngineConfig
{
    WindowConfig window;
    GraphicsConfig graphics;
    PhysicsConfig physics;
};