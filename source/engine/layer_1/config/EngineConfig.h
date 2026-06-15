#pragma once

#include "WindowDesc.h"
#include "GraphicsDesc.h"
#include "PhysicsDesc.h"
#include "TimeDesc.h"

struct EngineConfig
{
    TimeDesc time;
    WindowDesc window;
    GraphicsDesc graphics;
    PhysicsDesc physics;
};