#include <windows.h>

#include "Log.h"
#include "Window.h"

static HDC deviceContext;
static HWND windowHandle;

Window* Window::s_instance = nullptr;

struct WindowsEvent {
    UINT message;
    WPARAM wParam;
    LPARAM lParam;
};

// Forward declarations
LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

static std::queue<std::pair<HWND, WindowsEvent>> g_messageQueue;


Window* Window::getInstance()
{
    return s_instance;
}

const uint32 Window::GetWidth()
{
	return m_width;
}

const uint32 Window::GetHeight()
{
    return m_height;
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
        m_width = width;
        m_height = height;
        //ENGINE_INFO(std::to_string(m_width));

        // update render view

        RECT rect;
        GetWindowRect(windowHandle, &rect);
        SetWindowPos(windowHandle, NULL, rect.left, rect.top, width, height, SWP_NOZORDER | SWP_NOMOVE);
    }
}

void Window::Initialize(WindowDesc config)
{
    if (s_instance != nullptr)
    {

    }
    else
    {
        s_instance = this;

        m_width = config.width;
        m_height = config.height;


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
            CW_USEDEFAULT, CW_USEDEFAULT, m_width, m_height,
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

// This will not work with multiple windows as i just pop them all.
std::vector<WindowEvent> Window::PollEvents()
{
    std::vector<WindowEvent> returnValue;

    MSG msg;

    // Flush the OS message queue; events are handled via g_messageQueue.
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        DispatchMessage(&msg);
    }

    // Handle queued events per window instance
    while (!g_messageQueue.empty())
    {
        auto [hwnd, ev] = g_messageQueue.front();
        g_messageQueue.pop();

        if (windowHandle == hwnd)
        {
            WindowEvent we{};
            switch (ev.message)
            {
                case WM_SETFOCUS:
                {
                    we.type = WindowEventType::FocusGained;
                    returnValue.push_back(we);
                    break;
                }
                case WM_KILLFOCUS:
                {
                    we.type = WindowEventType::FocusLost;
                    returnValue.push_back(we);
                    break;
                }
                case WM_SIZE:
                {
                    m_width = LOWORD(ev.lParam);
                    m_height = HIWORD(ev.lParam);
                    we.type = WindowEventType::Resize;
                    we.width = m_width;
                    we.height = m_height;

                    //Log::Error("S: " + std::to_string(m_width) + ":" + std::to_string(m_height));


                    returnValue.push_back(we);
                    break;
                }
                case WM_DESTROY:
                {
                    we.type = WindowEventType::Close;
                    returnValue.push_back(we);
                    UnInitialize();
                    break;
                }
                case WM_SETCURSOR:
                {
                    if ((HWND)ev.wParam == hwnd && LOWORD(ev.lParam) == HTCLIENT)
                    {
                        SetCursor(LoadCursor(nullptr, IDC_ARROW));
                    }
                    break;
                }
            }
        }
    }

    return returnValue;
}

static LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) 
{
    WindowsEvent ev{ message, wParam, lParam };
    g_messageQueue.push({ hwnd, ev });
    return DefWindowProc(hwnd, message, wParam, lParam);
}
