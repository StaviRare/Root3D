#include "PlatformDetector.h"

Platform PlatformDetector::_currentPlatform = Platform::Unknown;

void PlatformDetector::Initialize()
{
#if defined(_WIN32) || defined(_WIN64)
    _currentPlatform = Platform::Windows;
#else
    _currentPlatform = Platform::Unknown;
#endif
}

Platform PlatformDetector::GetPlatform()
{
    return _currentPlatform;
}

string PlatformDetector::GetPlatformName()
{
    if (_currentPlatform == Platform::Windows)
    {
        return "Windows";
    }
    else
    {
        return "Unknown";
    }
}
