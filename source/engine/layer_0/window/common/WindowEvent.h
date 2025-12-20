#pragma once

#include "Types.h"

enum class WindowEventType
{
    Resize,
    Close
};

struct WindowEvent
{
    WindowEventType type;
    uint32 width;
    uint32 height;
};