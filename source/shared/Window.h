#pragma once

#include "UserDefinedTypes.h"

class Window
{
    public:
    virtual void Initialize(int width, int height) = 0;
    virtual void UnInitialize() = 0;
    virtual void PollEvents() = 0;
    virtual void SwapBuffers() = 0;
    virtual void SetResolution(int width, int height) = 0;
    virtual void SetFullScreen(bool enable) = 0;

    int GetWidth() const { return width; }
    int GetHeight() const { return height; }
    void SetResizeFunction(Func2Arg<int, int> ptr) { onResizePtr = ptr; };

    protected:
    int width;
    int height;
    Func2Arg<int, int> onResizePtr = nullptr;
};