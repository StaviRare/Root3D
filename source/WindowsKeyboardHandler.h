#pragma once

#include "InputHandler.h"

class WindowsKeyboardHandler : public InputHandler {
public:
    bool keyDown(const std::string& key) override;
};
