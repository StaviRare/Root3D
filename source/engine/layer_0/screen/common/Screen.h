#pragma once

#include "UserDefinedTypes.h"

class Screen
{
    friend class Engine;

    public:
    static const int GetWidth();
    static const int GetHeight();
    static void SetFullScreen(bool enable);
    static void SetResolution(int width, int height);
    static void RegisterResizeCallback(Func2Arg<int, int> ptr);
    static void UnRegisterResizeCallback(Func2Arg<int, int> ptr);
    static void* GetNativeHandle();

    private:
    static void Initialize();
    static void UnInitialize();
    static void Resume();
    static void Pause();
    static void PollEvents();
};