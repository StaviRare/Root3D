#include "PlatformDetector.h"

Platform PlatformDetector::_currentPlatform = Platform::Unknown;

void PlatformDetector::Initialize()
{
#if defined(__ANDROID__)
    _currentPlatform = Platform::Android;
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
    if (_currentPlatform == Platform::Android)
    {
        return "Android";
    }
    else
    {
        return "Unknown";
    }
}
