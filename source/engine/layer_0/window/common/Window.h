#pragma once

#include "Types.h"
#include "UserDefinedTypes.h"

class Window
{
    friend class Engine;

    public:
    static Window& getInstance();
    const uint32 GetWidth();
    const uint32 GetHeight();
    void SetFullScreen(bool enable);
    void SetResolution(uint32 width, uint32 height);

    private:
    void Initialize(uint32 width, uint32 height);
    void UnInitialize();
    void Resume();
    void Pause();
    void PollEvents();
    void* GetNativeHandle();

    private:
    static Window* s_instance;
    uint32 m_width;
    uint32 m_height;
};