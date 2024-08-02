#include <iostream>

#include "Debug.h"
#include "Log.h"

void Debug::Log(const string& message)
{
    Log::Info(message);
}

void Debug::LogError(const string& errorMessage)
{
    Log::Error(errorMessage);
}

void Debug::LogWarning(const string& warningMessage)
{
    Log::Warning(warningMessage);
}