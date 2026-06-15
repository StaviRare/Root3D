#pragma once

#include "Types.h"

struct WindowDesc
{
    uint32 width = 800;
    uint32 height = 600;
    bool fullscreen = false;
    const char* title = "Root3D";
};