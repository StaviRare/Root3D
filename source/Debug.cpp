#include "Debug.h"

Debug::Debug() {}

void Debug::log(const std::string& message) {
    std::cout << message << std::endl;
}

void Debug::error(const std::string& errorMessage) {
    std::cout << "\033[1;31mError: " << errorMessage << "\033[0m" << std::endl;
}

void Debug::warning(const std::string& warningMessage) {
    std::cout << "\033[1;33mWarning: " << warningMessage << "\033[0m" << std::endl;
}