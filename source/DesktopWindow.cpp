#include "DesktopWindow.h"
#include "WindowWrapper.h"
#include "Debug.h"

static HWND hWnd;
static HDC hDC;

void DesktopWindow::Initialize(int w, int h)
{
    width = w;
    height = h;

    HINSTANCE hInstance = GetModuleHandle(nullptr);
    LPCWSTR className = L"MyWindowClass"; // Changed to wide-char string

    // Register the window class.
    WNDCLASS wc = {};
    wc.lpfnWndProc = DesktopWindow::StaticWindowProc; // Use StaticWindowProc here
    wc.hInstance = hInstance;
    wc.lpszClassName = className;

    RegisterClass(&wc);

    // Create the window.
    hWnd = CreateWindowEx(
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

    if (hWnd == NULL)
    {
        Debug::error("hWnd is NULL. Unable to proceed");
        return;
    }

    // Store the 'this' pointer in the window's user data
    SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

    ShowWindow(hWnd, SW_SHOW);

    hDC = GetDC(hWnd);  // Get and store the device context

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
    int pixelFormat = ChoosePixelFormat(hDC, &pfd);
    SetPixelFormat(hDC, pixelFormat, &pfd);

    // Create OpenGL rendering context
    HGLRC hRC = wglCreateContext(hDC);
    wglMakeCurrent(hDC, hRC);
}

void DesktopWindow::UnInitialize()
{
    // ToDo - OPEN GL code here. Handle better.
    // Get current rendering context
    HGLRC hRC = wglGetCurrentContext();

    // Release rendering context
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hRC);

    // Release device context
    ReleaseDC(hWnd, hDC);

    // Destroy window
    if (hWnd != NULL)
    {
        DestroyWindow(hWnd);
    }
}

void DesktopWindow::PollEvents()
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

void DesktopWindow::SwapBuffers()
{
    ::SwapBuffers(hDC);
}

void DesktopWindow::SetResolution(int width, int height)
{
    if (hWnd != nullptr)
    {
        RECT rect;
        GetWindowRect(hWnd, &rect);
        SetWindowPos(hWnd, NULL, rect.left, rect.top, width, height, SWP_NOZORDER | SWP_NOMOVE);
    }
}

void DesktopWindow::SetFullScreen(bool enable)
{
    static WINDOWPLACEMENT lastWindowPlacement = { sizeof(WINDOWPLACEMENT) };

    if (enable)
    {
        // Save the current placement if the window is not already in fullscreen
        if (GetWindowLong(hWnd, GWL_STYLE) & WS_OVERLAPPEDWINDOW)
        {
            GetWindowPlacement(hWnd, &lastWindowPlacement);
            SetWindowLong(hWnd, GWL_STYLE, WS_POPUP | WS_VISIBLE);
            SetWindowPos(hWnd, HWND_TOP, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), SWP_FRAMECHANGED);
        }
    }
    else
    {
        // Restore the window style
        SetWindowLongPtr(hWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW | WS_VISIBLE);

        // Restore the window placement
        lastWindowPlacement.showCmd = SW_SHOWNORMAL; // Force the window to restore to its normal state
        SetWindowPlacement(hWnd, &lastWindowPlacement);

        // Refresh window to apply the style and placement
        SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_FRAMECHANGED);
    }
}


LRESULT CALLBACK DesktopWindow::StaticWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (message == WM_NCCREATE)
    {
        // Set the instance pointer during window creation
        SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(reinterpret_cast<CREATESTRUCT*>(lParam)->lpCreateParams));
    }

    DesktopWindow* pThis = reinterpret_cast<DesktopWindow*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
    
    if (pThis)
    {
        // Forward the message to the instance's WindowProc method
        return pThis->WindowProc(message, wParam, lParam);
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}

LRESULT DesktopWindow::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
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

            if (onResizePtr)
            {
                onResizePtr(width, height);
            }
            else
            {
                Debug::error("Window resize call back was not set!");
            }

            break;
        }
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}