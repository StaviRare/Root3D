#include "Platform.h"

#if defined(__ANDROID__)
PlatformType Platform::s_current = PlatformType::Android;
#else
PlatformType Platform::s_current = PlatformType::Unknown;
#endif

string Platform::GetName()
{
    string returnValue = "Unknown";

    if (s_current == PlatformType::Android)
    {
        returnValue = "Android";
    }

    return returnValue;
}

PlatformType Platform::GetType()
{
    return s_current;
}