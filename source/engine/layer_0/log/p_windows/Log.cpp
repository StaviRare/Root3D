#include <iostream>
#include "Log.h"

void Log::Debug(const string& message)
{
    std::cout << message << std::endl;
}

void Log::Info(const string& message)
{
    std::cout << message << std::endl;
}

void Log::Error(const string& errorMessage)
{
    std::cout << "\033[1;31m" << errorMessage << "\033[0m" << std::endl;
}

void Log::Warning(const string& warningMessage)
{
    std::cout << "\033[1;33m" << warningMessage << "\033[0m" << std::endl;
}