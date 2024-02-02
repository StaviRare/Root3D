#pragma once

#include <windows.h>
#include "Window.h"

class DesktopWindow : public Window
{
    public:
    void Initialize(int width, int height) override;
    void UnInitialize() override;
    void PollEvents() override;
    void SwapBuffers() override;
    void SetResolution(int width, int height) override;
    void SetFullScreen(bool enable) override;

    private:
    static LRESULT CALLBACK StaticWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};