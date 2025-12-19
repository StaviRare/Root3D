#include <windows.h>
#include <vector>

#include "Log.h"
#include "Window.h"

static HDC deviceContext;
static HWND windowHandle;

Window* Window::s_instance = nullptr;

// Forward declarations
LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

Window& Window::getInstance()
{
    return *s_instance;
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
        // Restore the window style
        SetWindowLongPtr(windowHandle, GWL_STYLE, WS_OVERLAPPEDWINDOW | WS_VISIBLE);

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
        RECT rect;
        GetWindowRect(windowHandle, &rect);
        SetWindowPos(windowHandle, NULL, rect.left, rect.top, width, height, SWP_NOZORDER | SWP_NOMOVE);
    }
}

void Window::Initialize(uint32 width, uint32 height)
{
    if (s_instance != nullptr)
    {

    }
    else
    {
        s_instance = this;

        m_width = width;
        m_height = height;

        HINSTANCE hInstance = GetModuleHandle(nullptr);
        LPCWSTR className = L"MyWindowClass"; // Changed to wide-char string

                                              // Register the window class.
        WNDCLASS wc = {};
        wc.lpfnWndProc = WindowProc; // Use StaticWindowProc here
        wc.hInstance = hInstance;
        wc.lpszClassName = className;

        RegisterClass(&wc);

        // Create the window.
        windowHandle = CreateWindowEx(
            0,                                      // Optional window styles.
            className,                              // Window class
            L"Root3D",                              // Window title - wide-char string
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

        // Create OpenGL rendering context
        HGLRC hRC = wglCreateContext(deviceContext);
        wglMakeCurrent(deviceContext, hRC);
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
    // ToDo - OPEN GL code here. Handle better.
    // Get current rendering context
    HGLRC hRC = wglGetCurrentContext();

    // Release rendering context
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hRC);

    // Release device context
    ReleaseDC(windowHandle, deviceContext);

    // Destroy window
    if (windowHandle != NULL)
    {
        DestroyWindow(windowHandle);
    }
}

void Window::PollEvents()
{
    MSG msg;

    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        switch (msg.message)
        {
            case WM_SIZE:
            {
                Log::Debug("size");

                uint32 newWidth = LOWORD(msg.lParam);
                uint32 newHeight = HIWORD(msg.lParam);

                if (newWidth != m_width || newHeight != m_height)
                {
                    m_width = newWidth;
                    m_height = newHeight;

                    Log::Debug(std::to_string(m_width));

                }
                break;
            }

            case WM_QUIT:
            {
                UnInitialize();
                break;
            }
        }

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

void* Window::GetNativeHandle()
{
    return windowHandle;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        case WM_SETCURSOR:
        {
            if ((HWND)wParam == hwnd && LOWORD(lParam) == HTCLIENT)
            {
                SetCursor(LoadCursor(nullptr, IDC_ARROW));
                return TRUE;
            }
            break;
        }

        case WM_DESTROY:
        {
            //PostQuitMessage(0);
            return 0;
        }
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}
