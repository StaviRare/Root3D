#pragma once

#include "UserDefinedTypes.h"

class Screen
{
    friend class Core;

    public:
    static const int GetWidth();
    static const int GetHeight();
    static void SetFullScreen(bool enable);
    static void SetResolution(int width, int height);
    static void RegisterResizeCallback(Func2Arg<int, int> ptr);
    static void UnRegisterResizeCallback(Func2Arg<int, int> ptr);
    static void* GetNativeHandle();

    private:
    static void Initialize(int width, int height);
    static void UnInitialize();
    static void PollEvents();
    static void SwapFrameBuffers();
};