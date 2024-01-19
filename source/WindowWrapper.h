#pragma once

#include <vector>
#include "Window.h"
#include "DesktopWindow.h"

class WindowWrapper
{
    public:
    static int GetWidth();
    static int GetHeight();
    static void Initialize(int width, int height);
    static void UnInitialize();
    static void PollEvents();
    static void SwapBuffers();
    static void SetResolution(int width, int height);
    static void SetFullScreen(bool enable);
    static void RegisterResizeCallback(Func2Arg<int, int> ptr);
    static void UnRegisterResizeCallback(Func2Arg<int, int> ptr);

    private:
    static Window* window;
    static std::vector<Func2Arg<int, int>> callbacks;
    static void OnWindowResize(int width, int height);
};