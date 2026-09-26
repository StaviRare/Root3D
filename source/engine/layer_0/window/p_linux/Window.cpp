#include "Log.h"
#include "Window.h"
#include "PlatformEventQueue.h"

Window* Window::s_instance = nullptr;

Window* Window::getInstance()
{
    return s_instance;
}

const uint32 Window::GetWidth()
{
    return 0;
}

const uint32 Window::GetHeight()
{
    return 0;
}

void Window::SetFullScreen(bool enable)
{
    // Imp later
}

void Window::SetResolution(uint32 width, uint32 height)
{
    // Imp later
}

void Window::Initialize(WindowDesc config)
{
    // Imp later
}

void Window::Pause()
{
    // No need
}

void Window::Resume()
{
    // No need
}

void Window::UnInitialize()
{
    // Imp later
}

void* Window::GetNativeHandle()
{
    return nullptr;
}

void Window::PollEvents()
{
    // Imp later
}