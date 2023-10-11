#pragma once

#include <string>

class InputHandler {
public:
    virtual ~InputHandler() = default;

    // Abstract method to check if a key is pressed
    virtual bool keyDown(const std::string& key) = 0;
};
