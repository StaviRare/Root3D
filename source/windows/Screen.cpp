#include <windows.h>
#include <vector>
#include "Screen.h"
#include "Debug.h"

static int width;
static int height;
static HDC deviceContext;
static HWND windowHandle;
static std::vector<Func2Arg<int, int>> callbacks;

// Forward declarations
LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);


const int Screen::GetWidth()
{
	return width;
}

const int Screen::GetHeight()
{
    return height;
}

void Screen::SetFullScreen(bool enable)
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

void Screen::SetResolution(int width, int height)
{
    if (windowHandle != nullptr)
    {
        RECT rect;
        GetWindowRect(windowHandle, &rect);
        SetWindowPos(windowHandle, NULL, rect.left, rect.top, width, height, SWP_NOZORDER | SWP_NOMOVE);
    }
}

void Screen::RegisterResizeCallback(Func2Arg<int, int> ptr)
{
	if (ptr && std::find(callbacks.begin(), callbacks.end(), ptr) == callbacks.end())
	{
		callbacks.push_back(ptr);
	}
}

void Screen::UnRegisterResizeCallback(Func2Arg<int, int> ptr)
{
	callbacks.erase(std::remove(callbacks.begin(), callbacks.end(), ptr), callbacks.end());
}

void Screen::Initialize(int w, int h)
{
    width = w;
    height = h;

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
        CW_USEDEFAULT, CW_USEDEFAULT, w, h,
        NULL,                                   // Parent window    
        NULL,                                   // Menu
        hInstance,                              // Instance handle
        NULL                                    // Additional application data
    );

    if (windowHandle == NULL)
    {
        Debug::LogError("hWnd is NULL. Unable to proceed");
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

void Screen::UnInitialize()
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

void Screen::PollEvents()
{
    MSG msg;

    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);

        if (msg.message == WM_QUIT)
        {
            UnInitialize();
            break;
        }
    }
}

void Screen::SwapFrameBuffers()
{
    SwapBuffers(deviceContext);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {

    switch (message)
    {
        case WM_DESTROY:
        {
            PostQuitMessage(0);
            return 0;
        }
        case WM_SIZE:
        {
            width = LOWORD(lParam);
            height = HIWORD(lParam);

            for (auto& funcPtr : callbacks)
            {
                if (funcPtr)
                {
                    funcPtr(width, height);
                }
            }

            break;
        }
    }

    return DefWindowProc(hwnd, message, wParam, lParam);
}
