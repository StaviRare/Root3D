#include "Platform.h"

PlatformType Platform::s_current = PlatformType::Linux;

string Platform::GetName()
{
    string returnValue = "Unknown";

    if (s_current == PlatformType::Linux)
    {
        returnValue = "Linux";
    }

    return returnValue;
}

PlatformType Platform::GetType()
{
    return s_current;
}