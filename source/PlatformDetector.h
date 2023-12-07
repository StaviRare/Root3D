#pragma once

#include <iostream>
#include "Platform.h"

class PlatformDetector {
public:
    static Platform GetPlatform();
    static std::string GetPlatformName();
    static void Initialize();

private:
    static Platform _currentPlatform;
};