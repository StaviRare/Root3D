#include "SystemPlatform.h"

#if defined(_WIN32) || defined(_WIN64)
Platform SystemPlatform::s_current = Platform::Windows;
#else
Platform SystemPlatform::s_current = Platform::Unknown;
#endif

Platform SystemPlatform::Get()
{
    return s_current;
}

string SystemPlatform::GetName()
{
    string returnValue = "Unknown";

    if (s_current == Platform::Windows)
    {
        returnValue = "Windows";
    }

    return returnValue;
}
