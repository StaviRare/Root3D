#pragma once

#include <iostream>
#include <string>

class Debug {
public:
    Debug();

    static void log(const std::string& message);
    static void error(const std::string& errorMessage);
    static void warning(const std::string& warningMessage);
};
