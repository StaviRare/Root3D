#pragma once

#include "Types.h"
#include "IPlatformInput.h"

class Input
{
    public:
    static void Initialize(); // This should be protected
    static void Tick(); // This should be protected
    static void UnInitialize(); // This should be protected

    static bool GetKey(const string& key);
    static bool GetKeyDown(const string& key);
    static bool GetKeyUp(const string& key);

    private:
    static IPlatformInput* inputHandler;
};
