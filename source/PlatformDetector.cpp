#include "PlatformDetector.h"

Platform PlatformDetector::_currentPlatform = Platform::Unknown;

void PlatformDetector::Initialize() 
{
#ifdef _WIN32 || defined(_WIN64)
    _currentPlatform = Platform::Windows;
#elif __ANDROID__
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
    switch (_currentPlatform)
    {
    case Platform::Windows:
        return "Windows";
    case Platform::Android:
        return "Android";
    default:
        return "Unknown";
    }
}
