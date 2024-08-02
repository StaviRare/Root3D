#pragma once

#include "Types.h"

class Debug
{
    public:
    static void Log(const string& message);
    static void LogError(const string& errorMessage);
    static void LogWarning(const string& warningMessage);
};
