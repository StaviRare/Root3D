#include <Windows.h>

#include "WindowsKeyboardHandler.h"

bool WindowsKeyboardHandler::keyDown(const std::string& key) {
    if (key.length() != 1) {
        return false; // This example handles single character keys for simplicity
    }

    char k = key[0];
    int virtualKey = VkKeyScanA(k);

    if (virtualKey == -1) {
        return false; // Invalid key
    }

    return GetAsyncKeyState(virtualKey) & 0x8000;
}
