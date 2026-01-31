#include <windows.h>

#include "Log.h"
#include "Window.h"
#include "PlatformEventQueue.h"

static HDC deviceContext;
static HWND windowHandle;

Window* Window::s_instance = nullptr;

// Forward declarations
LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

Window* Window::getInstance()
{
    return s_instance;
}

const uint32 Window::GetWidth()
{
    if (!windowHandle) return 0;
    RECT rect;
    if (GetClientRect(windowHandle, &rect))
    {
        return static_cast<uint32>(rect.right - rect.left);
    }
    return 0;
}

const uint32 Window::GetHeight()
{
    if (!windowHandle) return 0;
    RECT rect;
    if (GetClientRect(windowHandle, &rect))
    {
        return static_cast<uint32>(rect.bottom - rect.top);
    }
    return 0;
}

void Window::SetFullScreen(bool enable)
{
    static WINDOWPLACEMENT lastWindowPlacement = { sizeof(WINDOWPLACEMENT) };

    if (enable)
    {
        // Save the current placement if the window is not already in fullscreen
        if (GetWindowLong(windowHandle, GWL_STYLE) & WS_OVERLAPPEDWINDOW)
        {
            GetWindowPlacement(windowHandle, &lastWindowPlacement);
            SetWindowLong(windowHandle, GWL_STYLE, WS_POPUP | WS_VISIBLE);
            SetWindowPos(windowHandle, HWND_TOP, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), SWP_FRAMECHANGED);
        }
    }
    else
    {
        // Restore the window placement
        lastWindowPlacement.showCmd = SW_SHOWNORMAL; // Force the window to restore to its normal state
        SetWindowPlacement(windowHandle, &lastWindowPlacement);

        // Refresh window to apply the style and placement
        SetWindowPos(windowHandle, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_FRAMECHANGED);
    }
}

void Window::SetResolution(uint32 width, uint32 height)
{
    if (windowHandle != nullptr)
    {
        //RECT rect;
        //GetWindowRect(windowHandle, &rect);
        //SetWindowPos(windowHandle, NULL, rect.left, rect.top, width, height, SWP_NOZORDER | SWP_NOMOVE);
    }
}

void Window::Initialize(WindowDesc config)
{
    if (s_instance != nullptr)
    {
        ENGINE_ERROR("Window already initialized.");
    }
    else
    {
        s_instance = this;

        HINSTANCE hInstance = GetModuleHandle(nullptr);
        LPCWSTR className = L"MyWindowClass";
        WNDCLASS wc = {};
        wc.lpfnWndProc = WindowProc;
        wc.hInstance = hInstance;
        wc.lpszClassName = className;

        RegisterClass(&wc);

        // title
        std::string title = config.title;
        std::wstring wtitle(title.begin(), title.end());

        // Create the window.
        windowHandle = CreateWindowEx(
            0,                                      // Optional window styles.
            className,                              // Window class
            wtitle.c_str(),                         // Window title - wide-char string
            WS_OVERLAPPEDWINDOW,                    // Window style
            CW_USEDEFAULT, CW_USEDEFAULT, config.width, config.height,
            NULL,                                   // Parent window    
            NULL,                                   // Menu
            hInstance,                              // Instance handle
            NULL                                    // Additional application data
        );

        if (windowHandle == NULL)
        {
            ENGINE_ERROR("hWnd is NULL. Unable to proceed");
            return;
        }

        // this is good but we have no use for it at the moment (can use getWindowLongPtr)
        //SetWindowLongPtr(windowHandle, GWL_STYLE, WS_OVERLAPPEDWINDOW | WS_VISIBLE);


        ShowWindow(windowHandle, SW_SHOW);

        deviceContext = GetDC(windowHandle);  // Get and store the device context

        // ToDo! - Make room for DirectX handling.
        // This should not be in initialized. Can change API after initialize.
        // OPEN GL CONTEXT // Check for current Graphics API
        PIXELFORMATDESCRIPTOR pfd = {};
        pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
        pfd.nVersion = 1;
        pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
        pfd.iPixelType = PFD_TYPE_RGBA;
        pfd.cColorBits = 24;
        pfd.cDepthBits = 16;
        pfd.iLayerType = PFD_MAIN_PLANE;

        // Set pixel format
        int pixelFormat = ChoosePixelFormat(deviceContext, &pfd);
        SetPixelFormat(deviceContext, pixelFormat, &pfd);

        // ToDo - Should be better.
        if (config.fullscreen)
        {
            SetFullScreen(true);
        }
    }
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
    // Release device context
    ReleaseDC(windowHandle, deviceContext);

    // Destroy window
    if (windowHandle != NULL)
    {
        DestroyWindow(windowHandle);
    }
}

void* Window::GetNativeHandle()
{
    return windowHandle;
}

void Window::PollEvents()
{
    MSG msg;

    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        DispatchMessage(&msg);
    }
}

static LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) 
{
    switch (message)
    {
        case WM_SIZE:
        {
            PlatformEvent ev;
            ev.type = EventType::Resize;
            ev.width = LOWORD(lParam);
            ev.height = HIWORD(lParam);
            PlatformEventQueue::Push(ev);
            break;
        }
        case WM_SETFOCUS:
        {
            PlatformEvent ev;
            ev.type = EventType::FocusGained;
            PlatformEventQueue::Push(ev);
            break;
        }
        case WM_KILLFOCUS:
        {
            PlatformEvent ev;
            ev.type = EventType::FocusLost;
            PlatformEventQueue::Push(ev);
            break;
        }
        case WM_CLOSE:
        {
            PlatformEvent ev;
            ev.type = EventType::Close;
            PlatformEventQueue::Push(ev);
            break;
        }
        case WM_SETCURSOR:
        {
            if (LOWORD(lParam) == HTCLIENT)
            {
                SetCursor(LoadCursor(nullptr, IDC_ARROW));
                return TRUE;
            }
            break;
        }
    }

    return DefWindowProc(hwnd, message, wParam, lParam);
}
