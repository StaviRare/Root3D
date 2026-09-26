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

void Log::Error(const string& message)
{
    std::cout << "\033[1;31m" << message << "\033[0m" << std::endl;
}

void Log::Warning(const string& message)
{
    std::cout << "\033[1;33m" << message << "\033[0m" << std::endl;
}