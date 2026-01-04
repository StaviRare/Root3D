#pragma once

#include <queue>
#include <mutex>

#include "Types.h"
#include "PlatformType.h"

class Platform
{
    public:
    static string GetName();
    static PlatformType GetType();

    private:
    static PlatformType s_current;
};