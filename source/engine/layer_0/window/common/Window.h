#pragma once

#include <queue>

#include "Types.h"
#include "WindowConfig.h"
#include "WindowEvent.h"

class Window
{
    friend class Engine;

    public:
    static Window* getInstance();
    const uint32 GetWidth();
    const uint32 GetHeight();
    void SetFullScreen(bool enable);
    void SetResolution(uint32 width, uint32 height);

    private:
    void Initialize(WindowConfig desc);
    void UnInitialize();
    void Resume();
    void Pause();
    void PollEvents();
    void* GetNativeHandle();

    private:
    static Window* s_instance;
    uint32 m_width;
    uint32 m_height;
    std::queue<WindowEvent> events;
};