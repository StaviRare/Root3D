#pragma once

#include "WindowsKeyboardHandler.h"

class Input {
public:
    static bool keyDown(const std::string& key) {
        // we can expand it with platform check and device check.
        static WindowsKeyboardHandler handler;
        return handler.keyDown(key);
    }
};
