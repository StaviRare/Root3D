#pragma once

#include "Types.h"
#include "Platform.h"

class PlatformDetector
{
    friend class EngineRuntime;

    public:
    static Platform GetPlatform();
    static string GetPlatformName();

    private:
    static void Initialize();

    private:
    static Platform _currentPlatform;
};