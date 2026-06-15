#include "Platform.h"

#if defined(_WIN32) || defined(_WIN64)
PlatformType Platform::s_current = PlatformType::Windows;
#else
PlatformType Platform::s_current = PlatformType::Unknown;
#endif

string Platform::GetName()
{
    string returnValue = "Unknown";

    if (s_current == PlatformType::Windows)
    {
        returnValue = "Windows";
    }

    return returnValue;
}

PlatformType Platform::GetType()
{
    return s_current;
}