#pragma once

#include "Types.h"

class Input
{
    friend class EngineRuntime;

    public:
    static bool GetKey(const string& key);
    static bool GetKeyDown(const string& key);
    static bool GetKeyUp(const string& key);

    private:
    static void Initialize();
    static void Tick();
    static void UnInitialize();
};
