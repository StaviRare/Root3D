#pragma once

#include "Types.h"
#include "Platform.h"

class SystemPlatform
{
    public:
    static Platform Get();
    static string GetName();

    private:
    static Platform s_current;
};