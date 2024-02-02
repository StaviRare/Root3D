#include "WindowWrapper.h"
#include "PlatformDetector.h"
#include "DesktopWindow.h"
#include "Platform.h"

Window* WindowWrapper::window = nullptr;
std::vector<Func2Arg<int, int>> WindowWrapper::callbacks;

int WindowWrapper::GetWidth()
{
    if (window)
    {
        return window->GetWidth();
    }
    else
    {
        return 0;
    }
}

int WindowWrapper::GetHeight()
{
    if (window)
    {
        return window->GetHeight();
    }
    else
    {
        return 0;
    }
}

void WindowWrapper::RegisterResizeCallback(Func2Arg<int, int> ptr)
{
    if (ptr && std::find(callbacks.begin(), callbacks.end(), ptr) == callbacks.end())
    {
        callbacks.push_back(ptr);
    }
}

void WindowWrapper::UnRegisterResizeCallback(Func2Arg<int, int> ptr)
{
    callbacks.erase(std::remove(callbacks.begin(), callbacks.end(), ptr), callbacks.end());
}

void WindowWrapper::Initialize(int width, int height)
{
    Platform platform = PlatformDetector::GetPlatform();

    switch (platform)
    {
        case Platform::Android:
        //window = new AndroidWindow();
        break;

        case Platform::Windows:
        window = new DesktopWindow();
        break;
    }

    if (window)
    {
        // Set resize callback pre-init for early events
        window->SetResizeFunction(OnWindowResize);
        window->Initialize(width, height);
    }
}

void WindowWrapper::UnInitialize()
{
    if (window)
    {
        window->UnInitialize();
    }
}

void WindowWrapper::PollEvents()
{
    if (window)
    {
        window->PollEvents();
    }
}

void WindowWrapper::SwapBuffers()
{
    if (window)
    {
        window->SwapBuffers();
    }
}

void WindowWrapper::SetResolution(int width, int height)
{
    if (window)
    {
        window->SetResolution(width, height);
    }
}

void WindowWrapper::SetFullScreen(bool enable)
{
    if (window)
    {
        window->SetFullScreen(enable);
    }
}

void WindowWrapper::OnWindowResize(int w, int h)
{
    for (auto& funcPtr : callbacks)
    {
        if (funcPtr)
        {
            funcPtr(w, h);
        }
    }
}