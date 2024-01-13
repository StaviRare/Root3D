#pragma once

#include "Types.h"
#include "Platform.h"

class PlatformDetector {
public:
    static Platform GetPlatform();
    static string GetPlatformName();
    static void Initialize();

private:
    static Platform _currentPlatform;
};