#include <iostream>
#include "Debug.h"

Debug::Debug() {}

void Debug::Log(const string& message)
{
    std::cout << message << std::endl;
}

void Debug::LogError(const string& errorMessage)
{
    std::cout << "\033[1;31mError: " << errorMessage << "\033[0m" << std::endl;
}

void Debug::LogWarning(const string& warningMessage)
{
    std::cout << "\033[1;33mWarning: " << warningMessage << "\033[0m" << std::endl;
}